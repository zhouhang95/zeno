#ifndef __NODESYS_COMMON_H__
#define __NODESYS_COMMON_H__

#include <zenomodel/include/modeldata.h>

enum ZenoGVItemType {
    ZTYPE_NODE = QGraphicsItem::UserType + 1,
    ZTYPE_LINK,
    ZTYPE_FULLLINK,
    ZTYPE_TEMPLINK,
    ZTYPE_SOCKET,
    ZTYPE_IMAGE,
    ZTYPE_PARAMWIDGET,
    ZTYPE_NODRAGITEM,
    ZTYPE_COLOR_CHANNEL,
    ZTYPE_COLOR_RAMP,
    ZTYPE_HEATMAP,
};

enum MIME_DATA_TYPE {
    MINETYPE_MULTI_NODES = 2099,
};

enum SEARLCH_ELEMENT {
    SEARCH_NAME,
    SEARCH_OBJNAME,
    SEARCH_PARAM,
};

enum SEARCH_RANGE {
    RG_CURRENT_SUBGRAPH,
    RG_ALLGRAPHS,
};

struct SEARCH_RECORD {
    QString id;
    QPointF pos;
};

struct NODES_MIME_DATA : public QObjectUserData
{
    QModelIndexList nodes;
    QPersistentModelIndex m_fromSubg;
};

enum STATUS_BTN
{
	STATUS_ONCE,
    STATUS_MUTE,
    STATUS_VIEW,
};

#define PIXELS_IN_CELL 8
#define SCENE_GRID_SIZE 100

#define SCENE_INIT_WIDTH 256000
#define SCENE_INIT_HEIGHT 128000

#endif