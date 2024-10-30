#include <QApplication>

#include "QGoodWindow"
#include "mainwindow.h"
#include "qfonticon.h"

int main(int argc, char *argv[])
{
#ifdef DESKTOP_INTERACTION_MODE
    QGoodWindow::setup();
#endif
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication application(argc, argv);

    // set application name
    application.setApplicationName("FaceDetect Live");

    bool isDarkTheme = true;
    QFontIcon::addFont(":/icons/icons/fontawesome-webfont-v6.6.0-solid-900.ttf");
    QFontIcon::addFont(":/icons/icons/fontawesome-webfont-v6.6.0-brands-400.ttf");
    QFontIcon::instance()->setColor(isDarkTheme?Qt::white:Qt::black);

#if DESKTOP_INTERACTION_MODE
    if(isDarkTheme) {
        QGoodWindow::setAppDarkTheme();
    } else {
        QGoodWindow::setAppLightTheme();
    }

    MainWindow window(isDarkTheme);
#else
    CentralWidget window;
#endif

    window.show();
    return application.exec();
}
