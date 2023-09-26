
#include "QUnifiedGraphicsView.h"
#include <QLabel>
#include <QMovie>
#include <QPainterPath>
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

    this->currentShownPreviewItem = PreviewImageItem::NONE;
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

    this->currentShownPreviewItem = PreviewImageItem::ORIGINAL;
}

void QUnifiedGraphicsView::addPreviewPixmap(const QPixmap& preview)
{
    this->previewPixmapItem = this->graphicsScene->addPixmap(preview);
    if (this->originalPixmapItem) {
        this->originalPixmapItem->hide();
    }
    this->centerOn(this->originalPixmapItem);
    this->currentShownPreviewItem = PreviewImageItem::PREVIEW;
}

void QUnifiedGraphicsView::removeOriginalPixmap()
{
    delete this->originalPixmapItem;
    this->originalPixmapItem = new QGraphicsPixmapItem();

    if (this->previewPixmapItem && !this->previewPixmapItem->isVisible()) {
        this->previewPixmapItem->show();
        this->currentShownPreviewItem = PreviewImageItem::PREVIEW;
    } else {
        this->currentShownPreviewItem = PreviewImageItem::NONE;
    }
}

void QUnifiedGraphicsView::removePreviewPixmap()
{
    delete this->previewPixmapItem;
    this->previewPixmapItem = new QGraphicsPixmapItem();

    if (this->originalPixmapItem && !this->originalPixmapItem->isVisible()) {
        this->originalPixmapItem->show();
        this->currentShownPreviewItem = PreviewImageItem::ORIGINAL;
    } else {
        this->currentShownPreviewItem = PreviewImageItem::NONE;
    }
}

void QUnifiedGraphicsView::clearPixmaps()
{
    this->removeOriginalPixmap();
    this->removePreviewPixmap();

    this->currentShownPreviewItem = PreviewImageItem::NONE;
}

void QUnifiedGraphicsView::swap()
{
    if (this->currentShownPreviewItem == PreviewImageItem::ORIGINAL) {
        this->showPreview();
    } else if (this->currentShownPreviewItem == PreviewImageItem::PREVIEW) {
        this->showOriginal();
    }
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
    if (this->previewPixmapItem && this->previewPixmapItem->isVisible()) {
        this->previewPixmapItem->hide();
    }

    if (this->originalPixmapItem && !this->originalPixmapItem->isVisible()) {
        this->originalPixmapItem->show();
        this->currentShownPreviewItem = PreviewImageItem::ORIGINAL;
    }
}

void QUnifiedGraphicsView::showPreview()
{
    if (this->originalPixmapItem && this->originalPixmapItem->isVisible()) {
        this->originalPixmapItem->hide();
    }

    if (this->previewPixmapItem && !this->previewPixmapItem->isVisible()) {
        this->previewPixmapItem->show();
        this->currentShownPreviewItem = PreviewImageItem::PREVIEW;
    }
}