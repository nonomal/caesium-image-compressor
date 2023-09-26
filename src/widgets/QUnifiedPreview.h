#ifndef CAESIUM_IMAGE_COMPRESSOR_QUNIFIEDPREVIEW_H
#define CAESIUM_IMAGE_COMPRESSOR_QUNIFIEDPREVIEW_H

#include "QUnifiedGraphicsView.h"
#include <QGridLayout>
#include <QWidget>

namespace Ui {
class QUnifiedPreview;
}

class QUnifiedPreview : public QWidget {
    Q_OBJECT
public:
    explicit QUnifiedPreview(QWidget* parent);
    ~QUnifiedPreview() override;

private:
    Ui::QUnifiedPreview* ui;

private slots:
    void onScaleFactorChanged(double scaleFactor);
    void onZoomSliderChanged(int value);

public:
    void addOriginalPixmap(const QPixmap& pixmap);
    void addPreviewPixmap(const QPixmap& pixmap);
    void setLoading(bool loading);
    void clear();
};

#endif // CAESIUM_IMAGE_COMPRESSOR_QUNIFIEDPREVIEW_H
