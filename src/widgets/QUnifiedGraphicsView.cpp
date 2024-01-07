
#include "QUnifiedGraphicsView.h"
#include <QLabel>
#include <QMovie>
#include <QScrollBar>

QUnifiedGraphicsView::QUnifiedGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{

    this->graphicsScene = new QGraphicsScene();
    this->setScene(this->graphicsScene);

    this->loaderLabel = new QLabel();
    this->loaderLabel->setAutoFillBackground(false);
    this->loaderLabel->setAttribute(Qt::WA_NoSystemBackground);
    this->loaderMovie = new QMovie(":/icons/ui/loader.webp");
    this->loaderMovie->setScaledSize(QSize(40, 40));
    this->loaderLabel->setMovie(this->loaderMovie);
    this->loaderMovie->start();
    this->loaderProxyWidget = this->graphicsScene->addWidget(this->loaderLabel);
    this->loaderProxyWidget->hide();

    this->originalPixmapItem = new QGraphicsPixmapItem();
    this->previewPixmapItem = new QGraphicsPixmapItem();
}

void QUnifiedGraphicsView::wheelEvent(QWheelEvent* event)
{
    if (!this->zoomEnabled) {
        return;
    }
    this->zooming = true;
    this->setScaleFactor(event);
    // emit scaleFactorChanged(event);
    this->zooming = false;
    emit this->horizontalScrollBar()->valueChanged(this->horizontalScrollBar()->value());
    emit this->verticalScrollBar()->valueChanged(this->verticalScrollBar()->value());
}

void QUnifiedGraphicsView::setScaleFactor(QWheelEvent* event)
{
    const ViewportAnchor anchor = transformationAnchor();
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    int angle = event->angleDelta().y();

    qreal factor = 1;
    if ((float)angle > WHEEL_TOLERANCE) {
        factor = ZOOM_IN_RATIO;
    } else if ((float)angle < -WHEEL_TOLERANCE) {
        factor = ZOOM_OUT_RATIO;
    }

    double expectedScaleFactor = std::clamp(this->scaleFactor * factor, MAX_ZOOM_OUT / 100, MAX_ZOOM_IN / 100);
    if (expectedScaleFactor == this->scaleFactor) {
        return;
    }

    setTransformationAnchor(anchor);
    this->scaleFactor *= factor;
    scale(factor, factor);

    emit scaleFactorChanged(this->scaleFactor);
}

void QUnifiedGraphicsView::setScaleFactor(double factor)
{
    this->resetTransform();
    this->scale(factor / 100, factor / 100);
    this->scaleFactor = factor / 100;
    emit scaleFactorChanged(this->scaleFactor);
}

void QUnifiedGraphicsView::resetScaleFactor()
{
    this->scaleFactor = 1;
    emit scaleFactorChanged(this->scaleFactor);
}

void QUnifiedGraphicsView::setLoading(bool l)
{
    this->loading = l;

    if (l) {
        this->graphicsScene->setSceneRect(0, 0, 40, 40);
        this->fitInView(this->rect(), Qt::KeepAspectRatio);
        this->loaderProxyWidget->show();
    } else {
        this->loaderProxyWidget->hide();
    }
}

void QUnifiedGraphicsView::setZoomEnabled(bool l)
{
    this->zoomEnabled = l;
}

void QUnifiedGraphicsView::addOriginalPixmap(const QPixmap& original)
{
    this->setScaleFactor(100.0);
    this->originalPixmapItem = this->graphicsScene->addPixmap(original);
    this->graphicsScene->setSceneRect(this->graphicsScene->itemsBoundingRect());
    this->centerOn(this->originalPixmapItem);
    emit currentPreviewImageChanged(PreviewImageItem::ORIGINAL);
}

void QUnifiedGraphicsView::addPreviewPixmap(const QPixmap& preview)
{
    this->previewPixmapItem = this->graphicsScene->addPixmap(preview.scaled(this->originalPixmapItem->pixmap().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    if (this->originalPixmapItem) {
        this->originalPixmapItem->hide();
    }
    this->centerOn(this->originalPixmapItem);
    emit currentPreviewImageChanged(PreviewImageItem::PREVIEW);
}

void QUnifiedGraphicsView::removeOriginalPixmap()
{
    delete this->originalPixmapItem;
    this->originalPixmapItem = new QGraphicsPixmapItem();
}

void QUnifiedGraphicsView::removePreviewPixmap()
{
    delete this->previewPixmapItem;
    this->previewPixmapItem = new QGraphicsPixmapItem();
}

void QUnifiedGraphicsView::clearPixmaps()
{
    this->removeOriginalPixmap();
    this->removePreviewPixmap();

    emit currentPreviewImageChanged(PreviewImageItem::NONE);
}

void QUnifiedGraphicsView::zoomToFit()
{
    double ratioW = this->rect().width() / this->graphicsScene->width();
    double ratioH = this->rect().height() / this->graphicsScene->height();

    double ratio = std::min(ratioW, ratioH);

    this->setScaleFactor(100 * ratio);
}

void QUnifiedGraphicsView::zoomToFull()
{
    this->setScaleFactor(100.0);
}

void QUnifiedGraphicsView::showOriginal()
{
    if (this->originalPixmapItem && !this->originalPixmapItem->isVisible()) {
        if (this->previewPixmapItem && this->previewPixmapItem->isVisible()) {
            this->previewPixmapItem->hide();
        }
        this->originalPixmapItem->show();
        emit currentPreviewImageChanged(PreviewImageItem::ORIGINAL);
    }
}

void QUnifiedGraphicsView::showPreview()
{
    if (this->previewPixmapItem && !this->previewPixmapItem->isVisible()) {
        if (this->originalPixmapItem && this->originalPixmapItem->isVisible()) {
            this->originalPixmapItem->hide();
        }
        this->previewPixmapItem->show();
        emit currentPreviewImageChanged(PreviewImageItem::PREVIEW);
    }
}

//void QUnifiedGraphicsView::drawForeground(QPainter* painter, const QRectF& rect)
//{
//    QGraphicsView::drawForeground(painter, rect);
//
//    QString overlayText = "Prova";
//    // Draw overlay text in the middle of the view
//    painter->setRenderHint(QPainter::TextAntialiasing);
//    painter->setPen(Qt::black); // Set text color
//    QFont font = painter->font();
//    font.setPointSize(56); // Set font size
//    painter->setFont(font);
//
//    QFontMetrics fm(painter->font());
//    // Calculate the position to center the text
//    QPointF center = mapToScene(viewport()->rect().center());
//    qDebug() << center;
//    qDebug() << fm.boundingRect(overlayText).width();
//    qDebug() << fm.boundingRect(overlayText).height();
//    QPointF textPosition = center - QPointF(fm.boundingRect(overlayText).width() / 2, fm.boundingRect(overlayText).height() / 2);
//
//    // Draw the overlay text
//    painter->drawText(textPosition, overlayText);
//}
