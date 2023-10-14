#ifndef CAESIUM_IMAGE_COMPRESSOR_QUNIFIEDPREVIEW_H
#define CAESIUM_IMAGE_COMPRESSOR_QUNIFIEDPREVIEW_H

#include "QUnifiedGraphicsView.h"
#include "utils/Utils.h"
#include <QGridLayout>
#include <QWidget>

namespace Ui {
class QUnifiedPreview;
}

class QUnifiedPreview : public QWidget {
    Q_OBJECT

public:
    [[maybe_unused]] explicit QUnifiedPreview(QWidget* parent);
    ~QUnifiedPreview() override;

private:
    Ui::QUnifiedPreview* ui;

    ImagePreview originalPreview;
    ImagePreview compressedPreview;
    PreviewImageItem currentShownPreviewItem = PreviewImageItem::NONE;

private slots:
    void onScaleFactorChanged(double scaleFactor);
    void onZoomSliderChanged(int value);
    void onCurrentShowPreviewItemChanged(const PreviewImageItem& item);
    void swap();

public:
    void setOriginalPreview(const ImagePreview& originalPreview);
    void setCompressedPreview(const ImagePreview& compressedPreview);
    void setLoading(bool loading);
    void clear();
};

#endif // CAESIUM_IMAGE_COMPRESSOR_QUNIFIEDPREVIEW_H
