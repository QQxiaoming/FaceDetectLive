#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVideoSink>
#include <QCamera>
#include <QMediaCaptureSession>

#include "QGoodWindow"
#include "QGoodCentralWidget"

#include "qfacefusion_api.h"

namespace Ui {
class CentralWidget;
}

class CentralWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();

    void checkCameraPermission(void);
    void checkAuthorizationStatus(void);
    void checkCloseEvent(QCloseEvent *event);

private slots:
    void on_actionSrc_triggered();
    void on_actionAbout_triggered();

private:
    Ui::CentralWidget *ui;
    QCamera *camera;
    QMediaCaptureSession *capture;
    QVideoSink *videoSink;
    QTimer *timer;
    QTimer *timer2;
    QFaceFusionThread *faceSwapThread;
    QPixmap srcImg;
    QPixmap outImg;
    int64_t m_index = 0;
    bool donotCheckCloseEvent = false;
    bool hasSrc = false;
};


class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    explicit MainWindow(bool isDark = true, QWidget *parent = nullptr);
    ~MainWindow();
    void setLaboratoryButton(QToolButton *laboratoryButton) {
        QTimer::singleShot(0, this, [this, laboratoryButton](){
            laboratoryButton->setFixedSize(m_good_central_widget->titleBarHeight(),m_good_central_widget->titleBarHeight());
            m_good_central_widget->setRightTitleBarWidget(laboratoryButton, false);
            connect(m_good_central_widget,&QGoodCentralWidget::windowActiveChanged,this, [laboratoryButton](bool active){
                laboratoryButton->setEnabled(active);
            });
        });
    }
    void fixMenuBarWidth(void) {
        if (m_menu_bar) {
            /* FIXME: Fix the width of the menu bar 
             * please optimize this code */
            int width = 0;
            int itemSpacingPx = m_menu_bar->style()->pixelMetric(QStyle::PM_MenuBarItemSpacing);
            for (int i = 0; i < m_menu_bar->actions().size(); i++) {
                QString text = m_menu_bar->actions().at(i)->text();
                QFontMetrics fm(m_menu_bar->font());
                width += fm.size(0, text).width() + itemSpacingPx*1.5;
            }
            m_good_central_widget->setLeftTitleBarWidth(width);
        }
    }

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QGoodCentralWidget *m_good_central_widget;
    QMenuBar *m_menu_bar = nullptr;
    CentralWidget *m_central_widget;
};

#endif // MAINWINDOW_H
