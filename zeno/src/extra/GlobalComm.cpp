#include <zeno/extra/GlobalComm.h>
#include <zeno/extra/GlobalState.h>
#include <zeno/funcs/ObjectCodec.h>
#include <zeno/utils/log.h>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <cassert>

namespace zeno {

static void toDisk(std::string cachedir, int frameid, GlobalComm::ViewObjects &objs) {
    if (cachedir.empty()) return;
    std::vector<char> buf;
    std::vector<size_t> poses;
    std::string keys = "ZENCACHE" + std::to_string((int)objs.size());

    for (auto const &[key, obj]: objs) {
        keys.push_back('\a');
        keys.append(key);
        poses.push_back(buf.size());
        encodeObject(obj.get(), buf);
    }
    poses.push_back(buf.size());
    keys.push_back('\a');

    auto path = std::filesystem::u8path(cachedir) / (std::to_string(1000000 + frameid).substr(1) + ".zencache");
    log_critical("dump cache to disk {}", path);
    std::ofstream ofs(path, std::ios::binary);
    std::ostreambuf_iterator<char> oit(ofs);
    std::copy(keys.begin(), keys.end(), oit);
    std::copy_n((const char*)poses.data(), poses.size() * sizeof(size_t), oit);
    std::copy(buf.begin(), buf.end(), oit);
    objs.clear();
}

static bool fromDisk(std::string cachedir, int frameid, GlobalComm::ViewObjects &objs) {
    if (cachedir.empty()) return false;
    objs.clear();
    auto path = std::filesystem::u8path(cachedir) / (std::to_string(1000000 + frameid).substr(1) + ".zencache");
    log_critical("load cache from disk {}", path);

    auto szBuffer = std::filesystem::file_size(path);
    std::vector<char> dat(szBuffer);
    FILE* fp = fopen(path.string().c_str(), "rb");
    if (!fp) {
        log_error("zeno cache file does not exist");
        return false;
    }
    size_t ret = fread(&dat[0], 1, szBuffer, fp);
    assert(ret == szBuffer);
    fclose(fp);
    fp = nullptr;

    if (dat.size() <= 8 || std::string(dat.data(), 8) != "ZENCACHE") {
        log_error("zeno cache file broken (1)");
        return false;
    }
    size_t pos = std::find(dat.begin() + 8, dat.end(), '\a') - dat.begin();
    if (pos == dat.size()) {
        log_error("zeno cache file broken (2)");
        return false;
    }
    int keyscount = std::stoi(std::string(dat.data() + 8, pos - 8));
    pos = pos + 1;
    std::vector<std::string> keys;
    for (int k = 0; k < keyscount; k++) {
        size_t newpos = std::find(dat.begin() + pos, dat.end(), '\a') - dat.begin();
        if (newpos == dat.size()) {
            log_error("zeno cache file broken (3.{})", k);
            return false;
        }
        keys.emplace_back(dat.data() + pos, newpos - pos);
        pos = newpos + 1;
    }

    std::vector<size_t> poses(keyscount + 1);
    std::copy_n(dat.data() + pos, (keyscount + 1) * sizeof(size_t), (char *)poses.data());
    pos += (keyscount + 1) * sizeof(size_t);
    for (int k = 0; k < keyscount; k++) {
        if (poses[k] > dat.size() - pos || poses[k + 1] < poses[k]) {
            log_error("zeno cache file broken (4.{})", k);
            return true;
        }
        const char *p = dat.data() + pos + poses[k];
        objs.try_emplace(keys[k], decodeObject(p, poses[k + 1] - poses[k]));
    }
    return true;
}

ZENO_API void GlobalComm::newFrame() {
    std::lock_guard lck(m_mtx);
    log_debug("GlobalComm::newFrame {}", m_frames.size());
    m_frames.emplace_back().b_frame_completed = false;
}

ZENO_API void GlobalComm::finishFrame() {
    std::lock_guard lck(m_mtx);
    log_debug("GlobalComm::finishFrame {}", m_maxPlayFrame);
    if (m_maxPlayFrame >= 0 && m_maxPlayFrame < m_frames.size())
        m_frames[m_maxPlayFrame].b_frame_completed = true;
    m_maxPlayFrame += 1;
}

ZENO_API void GlobalComm::dumpFrameCache(int frameid) {
    std::lock_guard lck(m_mtx);
    int frameIdx = frameid - beginFrameNumber;
    if (frameIdx >= 0 && frameIdx < m_frames.size()) {
        log_debug("dumping frame {}", frameid);
        toDisk(cacheFramePath, frameid, m_frames[frameIdx].view_objects);
    }
}

ZENO_API void GlobalComm::addViewObject(std::string const &key, std::shared_ptr<IObject> object) {
    std::lock_guard lck(m_mtx);
    log_debug("GlobalComm::addViewObject {}", m_frames.size());
    if (m_frames.empty()) throw makeError("empty frame cache");
    m_frames.back().view_objects.try_emplace(key, std::move(object));
}

ZENO_API void GlobalComm::clearState() {
    std::lock_guard lck(m_mtx);
    m_frames.clear();
    m_inCacheFrames.clear();
    m_maxPlayFrame = 0;
    maxCachedFrames = 1;
    cacheFramePath = {};
}

ZENO_API void GlobalComm::frameCache(std::string const &path, int gcmax) {
    cacheFramePath = path;
    maxCachedFrames = gcmax;
}

ZENO_API void GlobalComm::frameRange(int beg, int end) {
    beginFrameNumber = beg;
    endFrameNumber = end;
}

ZENO_API int GlobalComm::maxPlayFrames() {
    std::lock_guard lck(m_mtx);
    return m_maxPlayFrame + beginFrameNumber; // m_frames.size();
}

ZENO_API GlobalComm::ViewObjects const *GlobalComm::getViewObjects(const int frameid) {
    int frameIdx = frameid - beginFrameNumber;
    std::lock_guard lck(m_mtx);
    if (frameIdx < 0 || frameIdx >= m_frames.size())
        return nullptr;
    if (maxCachedFrames != 0) {
        // load back one gc:
        if (!m_inCacheFrames.count(frameid)) {  // notinmem then cacheit
            bool ret = fromDisk(cacheFramePath, frameid, m_frames[frameIdx].view_objects);
            if (!ret)
                return nullptr;
            m_inCacheFrames.insert(frameid);
            // and dump one as balance:
            if (m_inCacheFrames.size() && m_inCacheFrames.size() > maxCachedFrames) { // notindisk then dumpit
                for (int i: m_inCacheFrames) {
                    if (i != frameid) {
                        // seems that objs will not be modified when load_objects called later.
                        // so, there is no need to dump.
                        //toDisk(cacheFramePath, i, m_frames[i - beginFrameNumber].view_objects);
                        m_frames[i - beginFrameNumber].view_objects.clear();
                        m_inCacheFrames.erase(i);
                        break;
                    }
                }
            }
        }
    }
    return &m_frames[frameIdx].view_objects;
}

ZENO_API GlobalComm::ViewObjects const &GlobalComm::getViewObjects() {
    std::lock_guard lck(m_mtx);
    return m_frames.back().view_objects;
}

ZENO_API bool GlobalComm::isFrameCompleted(int frameid) const {
    frameid -= beginFrameNumber;
    if (frameid < 0 || frameid >= m_frames.size())
        return false;
    return m_frames[frameid].b_frame_completed;
}

}
