#include "QUnifiedPreview.h"
#include "ui_QUnifiedPreview.h"

QUnifiedPreview::QUnifiedPreview(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::QUnifiedPreview)
{
    ui->setupUi(this);

    this->currentShownPreviewItem = PreviewImageItem::NONE;

    ui->swap_ToolButton->setIcon(QIcon(":/icons/ui/swap.svg"));
    ui->zoomFit_ToolButton->setIcon(QIcon(":/icons/ui/zoom_fit.svg"));
    ui->zoomFull_ToolButton->setIcon(QIcon(":/icons/ui/expand.svg"));


    connect(ui->zoomFactor_Slider, &QSlider::valueChanged, this, &QUnifiedPreview::onZoomSliderChanged);
    connect(ui->unified_GraphicsView, &QUnifiedGraphicsView::scaleFactorChanged, this, &QUnifiedPreview::onScaleFactorChanged);
    connect(ui->swap_ToolButton, &QToolButton::pressed, this, &QUnifiedPreview::swap);
    connect(ui->zoomFull_ToolButton, &QToolButton::pressed, ui->unified_GraphicsView, &QUnifiedGraphicsView::zoomToFull);
    connect(ui->zoomFit_ToolButton, &QToolButton::pressed, ui->unified_GraphicsView, &QUnifiedGraphicsView::zoomToFit);
    connect(ui->unified_GraphicsView, &QUnifiedGraphicsView::currentPreviewImageChanged, this, &QUnifiedPreview::onCurrentShowPreviewItemChanged);
}

QUnifiedPreview::~QUnifiedPreview()
{
    delete ui;
}

void QUnifiedPreview::clear()
{
    ui->unified_GraphicsView->clearPixmaps();
    ui->unified_GraphicsView->resetScaleFactor();
    ui->previewInfo_Label->clear();
}

void QUnifiedPreview::setLoading(bool loading)
{
    ui->unified_GraphicsView->setLoading(loading);
    ui->previewInfo_Label->clear();
    ui->previewInfo_Label->setText(tr("Loading..."));
}

void QUnifiedPreview::onZoomSliderChanged(int value) {
    double factor = std::clamp(value, 1, 500);
    ui->unified_GraphicsView->setScaleFactor((double)factor);
}

void QUnifiedPreview::onScaleFactorChanged(double scaleFactor)
{
    ui->zoomFactor_Slider->setValue(qRound(scaleFactor * 100));
}

void QUnifiedPreview::setOriginalPreview(const ImagePreview& op)
{
    QUnifiedPreview::originalPreview = op;
    ui->unified_GraphicsView->addOriginalPixmap(op.image);
    ui->previewInfo_Label->setText(QString("ORIGINAL - %1 %2").arg(toHumanSize((double)op.fileInfo.size()), op.format));
    this->currentShownPreviewItem = PreviewImageItem::ORIGINAL;
}

void QUnifiedPreview::setCompressedPreview(const ImagePreview& cp)
{
    QUnifiedPreview::compressedPreview = cp;
    ui->unified_GraphicsView->addPreviewPixmap(cp.image);
    ui->previewInfo_Label->setText(QString("PREVIEW - %1 %2").arg(toHumanSize((double)cp.fileInfo.size()), cp.format));
    this->currentShownPreviewItem = PreviewImageItem::PREVIEW;
}

void QUnifiedPreview::swap()
{
    if (this->currentShownPreviewItem == PreviewImageItem::ORIGINAL) {
        ui->unified_GraphicsView->showPreview();
    } else if (this->currentShownPreviewItem == PreviewImageItem::PREVIEW) {
        ui->unified_GraphicsView->showOriginal();
    }
}

void QUnifiedPreview::onCurrentShowPreviewItemChanged(const PreviewImageItem& item)
{
    this->currentShownPreviewItem = item;
    if (item == PreviewImageItem::ORIGINAL) {
        ui->previewInfo_Label->setText(QString("ORIGINAL - %1 %2").arg(toHumanSize((double)this->originalPreview.fileInfo.size()), this->originalPreview.format));
    } else if (item == PreviewImageItem::PREVIEW) {
        ui->previewInfo_Label->setText(QString("PREVIEW - %1 %2").arg(toHumanSize((double)this->compressedPreview.fileInfo.size()), this->compressedPreview.format));
    } else {
        ui->previewInfo_Label->clear();
    }
}
