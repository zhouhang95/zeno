#include "ztimeline.h"
#include "zslider.h"
#include "zenomainwindow.h"
#include "zenoapplication.h"
#include "viewport/viewportwidget.h"
#include <comctrl/zlabel.h>
#include <zenoui/style/zenostyle.h>
#include <zenoui/comctrl/effect/innershadoweffect.h>
#include <zeno/utils/envconfig.h>
#include <zenomodel/include/uihelper.h>
#include "ui_ztimeline.h"
#include "viewport/zenovis.h"
#include <zenovis/DrawOptions.h>
#include <iostream>

//////////////////////////////////////////////
ZTimeline::ZTimeline(QWidget* parent)
    : QWidget(parent)
{
    m_ui = new Ui::Timeline;
    m_ui->setupUi(this);
 
    setFocusPolicy(Qt::ClickFocus);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(42, 42, 42));
    setAutoFillBackground(true);
    setPalette(pal);

    int deflFrom = 0, deflTo = 0;
    m_ui->editFrom->setText(QString::number(deflFrom));
    m_ui->editTo->setText(QString::number(deflTo));
    m_ui->timeliner->setFromTo(deflFrom, deflTo);
    
    initStyleSheet();
    initSignals();
    initBtnIcons();

    if (zeno::envconfig::get("ALWAYS"))
        m_ui->btnAlways->setChecked(true);
}

void ZTimeline::initSignals()
{
    connect(m_ui->btnPlay, SIGNAL(clicked(bool)), this, SIGNAL(playForward(bool)));
    connect(m_ui->editFrom, SIGNAL(editingFinished()), this, SLOT(onFrameEditted()));
    connect(m_ui->editTo, SIGNAL(editingFinished()), this, SLOT(onFrameEditted()));
    connect(m_ui->timeliner, SIGNAL(sliderValueChange(int)), this, SIGNAL(sliderValueChanged(int)));
    connect(m_ui->btnRun, SIGNAL(clicked()), this, SIGNAL(run()));
    connect(m_ui->btnKill, SIGNAL(clicked()), this, SIGNAL(kill()));
    connect(m_ui->btnAlways, &QPushButton::clicked, this, [=](bool bChecked) {
        if (bChecked)
            emit alwaysChecked();
    });
    connect(m_ui->editSR, &QLineEdit::editingFinished, this, [=]() {
        auto srTime = std::abs(m_ui->editSR->text().toInt());  // Avoid negative
        //std::cout << "SR: SimpleRender " << srTime << "\n";
        auto viewport = zenoApp->getMainWindow()->getDisplayWidget()->getViewportWidget();
        auto scene = Zenovis::GetInstance().getSession()->get_scene();
        viewport->simpleRenderTime = srTime;
        if(srTime == 0){
            scene->drawOptions->simpleRender = false;
        }else{
            scene->drawOptions->simpleRender = true;
        }
        scene->drawOptions->needRefresh = true;
    });
    m_ui->btnAlways->setShortcut(QKeySequence("F1"));
    m_ui->btnRun->setShortcut(QKeySequence("F2"));
    m_ui->btnKill->setShortcut(QKeySequence("Shift+F2"));
    m_ui->btnBackward->setShortcut(QKeySequence("Shift+F3"));
    m_ui->btnForward->setShortcut(QKeySequence("F3"));
    m_ui->btnPlay->setShortcut(QKeySequence("F4"));
    connect(m_ui->btnBackward, &QPushButton::clicked, this, [=]() {
        int frame = m_ui->timeliner->value();
        auto ft = fromTo();
        int frameFrom = ft.first, frameTo = ft.second;
        if (frame > frameFrom && frameFrom >= 0)
        {
            m_ui->timeliner->setSliderValue(frame - 1);
        }
    });
    connect(m_ui->btnForward, &QPushButton::clicked, this, [=]() {
        int frame = m_ui->timeliner->value();
        auto ft = fromTo();
        int frameFrom = ft.first, frameTo = ft.second;
        if (frame < frameTo)
        {
            m_ui->timeliner->setSliderValue(frame + 1);
        }
    });
    connect(m_ui->editFrame, &QLineEdit::editingFinished, this, [=]() {
        int frame = m_ui->editFrame->text().toInt();
        m_ui->timeliner->setSliderValue(frame);
    });
    connect(this, &ZTimeline::sliderValueChanged, this, [=]() {
        QString numText = QString::number(m_ui->timeliner->value());
        m_ui->editFrame->setText(numText);
    });
    connect(m_ui->btnAlways, &QCheckBox::clicked, [=](){
        if (m_ui->btnAlways->isChecked()) {
            emit run();
        }
    });
}

void ZTimeline::initStyleSheet()
{
    auto editors = findChildren<QLineEdit *>(QString(), Qt::FindDirectChildrenOnly);
    for (QLineEdit *pLineEdit : editors) {
        pLineEdit->setProperty("cssClass", "FCurve-lineedit");
    }

    auto buttons = findChildren<QPushButton *>(QString(), Qt::FindDirectChildrenOnly);
    for (QPushButton* btn : buttons)
    {
        if (btn != m_ui->btnRun && btn != m_ui->btnKill)
            btn->setProperty("cssClass", "timeline");

        InnerShadowEffect *effect = new InnerShadowEffect;
        btn->setGraphicsEffect(effect);
    }
}

void ZTimeline::initBtnIcons()
{
    m_ui->btnKFrame->setIcon(QIcon(":/icons/timeline-curvemap.svg"));
    m_ui->btnBackward->setIcon(QIcon(":/icons/timeline-lastframe.svg"));
    m_ui->btnForward->setIcon(QIcon(":/icons/timeline-nextframe.svg"));
    m_ui->btnRecycle->setIcon(QIcon(":/icons/timeline-recycle.svg"));
    m_ui->btnRecord->setIcon(QIcon(":/icons/timeline-record.svg"));
}

void ZTimeline::onTimelineUpdate(int frameid)
{
    bool blocked = m_ui->timeliner->signalsBlocked();
    BlockSignalScope scope(m_ui->timeliner);
    BlockSignalScope scope2(m_ui->editFrame);
    m_ui->timeliner->setSliderValue(frameid);
    m_ui->editFrame->setText(QString::number(frameid));
}

void ZTimeline::setSliderValue(int frameid)
{
    m_ui->timeliner->setSliderValue(frameid);
}

void ZTimeline::setPlayButtonToggle(bool bToggle)
{
    m_ui->btnPlay->setChecked(bToggle);
}

void ZTimeline::onFrameEditted()
{
    if (m_ui->editFrom->text().isEmpty() || m_ui->editTo->text().isEmpty())
        return;
    int frameFrom = m_ui->editFrom->text().toInt();
    int frameTo = m_ui->editTo->text().toInt();
    if (frameTo >= frameFrom)
        m_ui->timeliner->setFromTo(frameFrom, frameTo);
}

QPair<int, int> ZTimeline::fromTo() const
{
    bool bOk = false;
    int frameFrom = m_ui->editFrom->text().toInt(&bOk);
    int frameTo = m_ui->editTo->text().toInt(&bOk);
    return { frameFrom, frameTo };
}

bool ZTimeline::isAlways() const
{
    return m_ui->btnAlways->isChecked();
}

void ZTimeline::initFromTo(int frameFrom, int frameTo)
{
    BlockSignalScope s1(m_ui->timeliner);
    BlockSignalScope s2(m_ui->editFrom);
    BlockSignalScope s3(m_ui->editTo);

    m_ui->editFrom->setText(QString::number(frameFrom));
    m_ui->editTo->setText(QString::number(frameTo));
    if (frameTo >= frameFrom)
        m_ui->timeliner->setFromTo(frameFrom, frameTo);
}

void ZTimeline::setAlways(bool bOn)
{
    m_ui->btnAlways->setChecked(bOn);
}

void ZTimeline::resetSlider()
{
    m_ui->timeliner->setSliderValue(0);
}

int ZTimeline::value() const
{
    return m_ui->timeliner->value();
}
