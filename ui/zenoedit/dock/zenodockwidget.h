#ifndef __ZENO_DOCKWIDGET_H__
#define __ZENO_DOCKWIDGET_H__

#if 0
#include <unordered_set>
#include <QtWidgets>

class ZenoMainWindow;
class ZDockTabWidget;

#define TEST_ZENO_API_TRIGGER

enum DOCK_TYPE
{
    DOCK_EMPTY,
	DOCK_VIEW,
	DOCK_EDITOR,
	DOCK_NODE_PARAMS,
	DOCK_NODE_DATA,
    DOCK_LOG,
    DOCK_LIGHTS,
};

class ZenoDockWidget : public QDockWidget
{
    Q_OBJECT
    typedef QDockWidget _base;

public:
    explicit ZenoDockWidget(const QString &title, QWidget *parent = nullptr,
                         Qt::WindowFlags flags = Qt::WindowFlags());
    explicit ZenoDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~ZenoDockWidget();

    void setWidget(DOCK_TYPE m_type, QWidget* widget);
    void onNodesSelected(const QModelIndex& subgIdx, const QModelIndexList& nodes, bool select);
    void onPrimitiveSelected(const std::unordered_set<std::string>& primids);
    void newFrameUpdate();
    DOCK_TYPE type() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    bool event(QEvent* event) override;

signals:
    void maximizeTriggered();
    void floatTriggered();
    void splitRequest(bool bHorzonal);
    void dockSwitchClicked(DOCK_TYPE);
    void nodesSelected(const QModelIndex& subgIdx, const QModelIndexList& nodes);

private slots:
    void onDockOptionsClicked();
    void onMaximizeTriggered();
    void onFloatTriggered();
#ifdef TEST_ZENO_API_TRIGGER
    void onTestAPI();
#endif

private:
    void init(ZenoMainWindow* pMainWin);
    bool isTopLevelWin();

    DOCK_TYPE m_type;
    Qt::WindowFlags m_oldFlags;
    Qt::WindowFlags m_newFlags;
    ZDockTabWidget* m_tabWidget;
};

#endif

#endif
