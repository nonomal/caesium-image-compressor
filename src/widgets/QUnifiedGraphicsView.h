
#ifndef CAESIUM_IMAGE_COMPRESSOR_QUNIFIEDGRAPHICSVIEW_H
#define CAESIUM_IMAGE_COMPRESSOR_QUNIFIEDGRAPHICSVIEW_H

#include "utils/Utils.h"
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QLabel>
#include <QMouseEvent>
#include <QMovie>
#include <QWheelEvent>

class QUnifiedGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit QUnifiedGraphicsView(QWidget* parent = nullptr);
    void wheelEvent(QWheelEvent* event) override;
    void resetScaleFactor();
    void setLoading(bool l);
    void setZoomEnabled(bool l);
    void addOriginalPixmap(const QPixmap& original);
    void addPreviewPixmap(const QPixmap& preview);
    void removeOriginalPixmap();
    void removePreviewPixmap();
    void clearPixmaps();

    double scaleFactor = 1;

protected:
    const double WHEEL_TOLERANCE = 1; // Experimental for touchpads
    const double ZOOM_IN_RATIO = 1.05;
    const double ZOOM_OUT_RATIO = 0.95;
    const double MAX_ZOOM_IN = 5;
    const double MAX_ZOOM_OUT = 0.1;
    bool zooming = false;
    bool loading = false;
    bool zoomEnabled = true;


    QLabel* loaderLabel;
    QMovie* loaderMovie;
    QGraphicsProxyWidget* loaderProxyWidget;
    QGraphicsScene* graphicsScene;
    QGraphicsPixmapItem* originalPixmapItem;
    QGraphicsPixmapItem* previewPixmapItem;

//    void drawForeground(QPainter* painter, const QRectF &rect) override;

public slots:
    void setScaleFactor(QWheelEvent* event);
    void setScaleFactor(double factor);
    void showOriginal();
    void showPreview();
    void zoomToFit();
    void zoomToFull();

signals:
    void scaleFactorChanged(double factor);
    void currentPreviewImageChanged(const PreviewImageItem& item);
};

#endif // CAESIUM_IMAGE_COMPRESSOR_QUNIFIEDGRAPHICSVIEW_H
