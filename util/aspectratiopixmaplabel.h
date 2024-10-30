#ifndef ASPECTRATIOPIXMAPLABEL_H
#define ASPECTRATIOPIXMAPLABEL_H

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>

class AspectRatioPixmapLabel : public QLabel {
    Q_OBJECT

public:
    explicit AspectRatioPixmapLabel(QWidget *parent = nullptr);
    void setPixmap(const QPixmap &);
    QPixmap pixmap() const;
    QPixmap currPixmap();
    void setName(const QString &name);
    void setEmphasized(bool emphasized){
        m_emphasized = emphasized;
        repaint();
    }
    bool isEmphasized() const {
        return m_emphasized;
    }
    void setHidePix(bool hidePix) {
        m_hidePix = hidePix;
        repaint();
    }
    void setBlurPix(bool blurPix, int blurRadius = 100) {
        m_blurPix = blurPix;
        m_blurRadius = blurRadius;
        repaint();
    }
    void setEnableScal(bool enable) {
        m_enableScal = enable;
        repaint();
    }
    void setOverwrite(const QString &str = QString()) {
        overwrite = str;
    }
    
public slots:
    void setScal(qreal scal, qreal scalPosX, qreal scalPosY, QPoint point) {
        m_scal = scal;
        m_scalPosX = scalPosX;
        m_scalPosY = scalPosY;
        this->point = point;
        repaint();
    }

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;

signals:
    void clicked();
    void doubleClicked();
    void scalChange(qreal scal, qreal scalPosX, qreal scalPosY, QPoint point);

private:
    QPixmap pix;
    QString overwrite;
    QString name;
    bool m_emphasized = false;
    bool m_hidePix = false;
    bool m_blurPix = false;
    int m_blurRadius = 100;
    bool m_enableScal = false;
    qreal m_scal = 1.0;
    qreal m_scalPosX = 0.5;
    qreal m_scalPosY = 0.5;
    QPoint point = QPoint(0, 0);
    QPoint startPos = QPoint(0, 0);
    QPoint endPos = QPoint(0, 0);
    bool left_click = false;
};

#endif // ASPECTRATIOPIXMAPLABEL_H
