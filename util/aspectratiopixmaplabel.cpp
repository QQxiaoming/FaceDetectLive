#include <QPainter>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsBlurEffect>
#include <QPalette>
#include "aspectratiopixmaplabel.h"

AspectRatioPixmapLabel::AspectRatioPixmapLabel(QWidget *parent) : QLabel(parent) {
    this->setMinimumSize(1, 1);
    setScaledContents(false);
    setEmphasized(false);
}

void AspectRatioPixmapLabel::setPixmap(const QPixmap &p) {
    pix = p;
    this->repaint();
}

QPixmap AspectRatioPixmapLabel::pixmap() const {
    return pix;
}
    
QPixmap AspectRatioPixmapLabel::currPixmap() {
    QPixmap currPixmap(this->size());
    this->render(&currPixmap);
    return currPixmap;
}

void AspectRatioPixmapLabel::setName(const QString &name) {
    this->name = name;
    this->repaint();
}

void AspectRatioPixmapLabel::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QPalette p = this->palette();
    QColor tc = p.color(QPalette::WindowText);

    if(pix.isNull()) {
        QPen pen(tc, 2);
        painter.setPen(pen);
        QString str("No Image");
        if(!overwrite.isEmpty()) {
            painter.drawRect(1, 1, this->width()-2, this->height()-2);
            str = overwrite;
        }
        QFont font = painter.font();
        font.setPointSize(15);
        painter.setFont(font);
        QFontMetrics fm(font);
        int textWidth = fm.horizontalAdvance(str);
        int textHeight = fm.height();
        int startX = (this->width() - textWidth) / 2;
        int startY = (this->height() - textHeight) / 2;
        painter.drawText(startX, startY, str);
    } else if(m_hidePix) {
        QPen pen(tc, 2);
        painter.setPen(pen);
        painter.drawRect(1, 1, this->width()-2, this->height()-2);
        QFont font = painter.font();
        font.setPointSize(15);
        painter.setFont(font);
        QFontMetrics fm(font);
        int textWidth = fm.horizontalAdvance("Hidden");
        int textHeight = fm.height();
        int startX = (this->width() - textWidth) / 2;
        int startY = (this->height() - textHeight) / 2;
        painter.drawText(startX, startY, "Hidden");
    } else {
        QPixmap prePix = pix;
        if(m_blurPix) {
            QGraphicsScene scene;
            QGraphicsPixmapItem item;
            item.setPixmap(prePix);
            QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
            blur->setBlurRadius(m_blurRadius);
            blur->setBlurHints(QGraphicsBlurEffect::AnimationHint);
            item.setGraphicsEffect(blur);
            scene.addItem(&item);
            QPixmap blurredPix(prePix.size());
            blurredPix.fill(Qt::transparent);
            QPainter blurPainter(&blurredPix);
            scene.render(&blurPainter, QRectF(), QRectF(0, 0, prePix.width(), prePix.height()));
            prePix = blurredPix;
        }
        if(m_enableScal) {
            QPixmap targetPix(prePix.size());
            targetPix.fill(Qt::transparent);
            QPainter targetPixPainter(&targetPix);
            QPoint targetPixTopLeft(0-prePix.width()*(m_scal-1.0)*m_scalPosX,0-prePix.height()*(m_scal-1.0)*m_scalPosY);
            targetPixPainter.drawPixmap(targetPixTopLeft+point*m_scal, prePix.scaled(prePix.width()*m_scal, prePix.height()*m_scal, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            QPixmap scaledPix = targetPix.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
            int startX = (this->width() - scaledPix.width()) / 2;
            int startY = (this->height() - scaledPix.height()) / 2;
            
            painter.drawPixmap(startX, startY, scaledPix);
        } else {
            QPixmap scaledPix = prePix.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
            int startX = (this->width() - scaledPix.width()) / 2;
            int startY = (this->height() - scaledPix.height()) / 2;
            
            painter.drawPixmap(startX, startY, scaledPix);
        }
    }

    if(!name.isEmpty()) {
        QFont font = painter.font();
    #if defined(Q_OS_WIN) && defined(Q_CC_MSVC)
        int fontId = QFontDatabase::addApplicationFont(QApplication::applicationDirPath() + "/inziu-iosevkaCC-SC-regular.ttf");
    #else
        int fontId = QFontDatabase::addApplicationFont(QStringLiteral(":/font/font/inziu-iosevkaCC-SC-regular.ttf"));
    #endif
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (fontFamilies.size() > 0) {
            font.setFamily(fontFamilies[0]);
        }
        font.setFixedPitch(true);
#if defined(Q_OS_MACOS)
        font.setPointSize(15);
#else
        font.setPointSize(9);
#endif
        painter.setFont(font);
        painter.setPen(Qt::black);
        if(this->height() >= 10*painter.fontMetrics().height()) {
            int fontSize = painter.fontMetrics().horizontalAdvance(name);
            if(fontSize <= this->width()) {
                int startX = (this->width() - fontSize) / 2;
                int startY = this->height() - painter.fontMetrics().height();
                painter.drawText(startX, startY, name);
            } else if(fontSize > this->width() && fontSize < 2*this->width()) {
                int startX = 3;
                int startY = this->height() - painter.fontMetrics().height()*2;
                QString line1 = name.left(name.size()/2);
                QString line2 = name.right(name.size()/2);
                painter.drawText(startX, startY, line1);
                painter.drawText(startX, startY + painter.fontMetrics().height(), line2);
            } else {
                int startX = 3;
                int startY = this->height() - painter.fontMetrics().height()*3;
                QString line1 = name.left(name.size()/3);
                QString line2 = name.mid(name.size()/3, name.size()/3);
                QString line3 = name.right(name.size()/3);
                painter.drawText(startX, startY, line1);
                painter.drawText(startX, startY + painter.fontMetrics().height(), line2);
                painter.drawText(startX, startY + 2*painter.fontMetrics().height(), line3);
            }
        }
    }

    if (m_emphasized) {
        QPen pen(Qt::blue, 2);
        painter.setPen(pen);
        painter.drawRect(1, 1, this->width()-2, this->height()-2); // Draw border inside the widget
    }
}

void AspectRatioPixmapLabel::mousePressEvent(QMouseEvent *event) {
    if( event->button() == Qt::LeftButton) {
        this->left_click = true;
        this->startPos = event->position().toPoint();
        emit clicked();
    } else if(event->button() == Qt::MiddleButton) {
        m_scal = 1.0;
        m_scalPosX = 0.5;
        m_scalPosY = 0.5;
        point = QPoint(0, 0);
        repaint();
        emit scalChange(m_scal, m_scalPosX, m_scalPosY, point);
    }
}

void AspectRatioPixmapLabel::mouseMoveEvent(QMouseEvent *event) {
    if( this->left_click) {
        this->endPos = event->position().toPoint() - this->startPos;
        this->point = this->point + this->endPos;
        this->startPos = event->position().toPoint();
        repaint();
        emit scalChange(m_scal, m_scalPosX, m_scalPosY, point);
    }
}

void AspectRatioPixmapLabel::mouseReleaseEvent(QMouseEvent *event) {
    if( event->button() == Qt::LeftButton) {
        this->left_click = false;
    }
}

void AspectRatioPixmapLabel::mouseDoubleClickEvent(QMouseEvent *event) {
    if( event->button() == Qt::LeftButton) {
        emit doubleClicked();
    }
}

void AspectRatioPixmapLabel::wheelEvent(QWheelEvent *event) {
    const qreal scaleMin = 0.1;
    const qreal scaleMax = 10.0;
    const qreal scaleStep = 0.1;
    if (event->angleDelta().y() > 0) {
        m_scal += scaleStep;
    } else {
        m_scal -= scaleStep;
    }
    qreal eventPosX = event->position().x();
    qreal eventPosY = event->position().y();
    m_scalPosX = eventPosX/this->width();
    m_scalPosY = eventPosY/this->height();
    m_scal = qBound(scaleMin, m_scal, scaleMax);
    repaint();
    emit scalChange(m_scal, m_scalPosX, m_scalPosY, point);
}

