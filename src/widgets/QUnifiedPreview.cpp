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
    this->originalPreview = {};
    this->compressedPreview = {};
}

void QUnifiedPreview::setLoading(const bool loading) const
{
    // ui->unified_GraphicsView->setLoading(loading);
    // ui->previewInfo_Label->clear();
    // if (loading) {
    //     ui->previewInfo_Label->setText(tr("Loading..."));
    // }
}

void QUnifiedPreview::onZoomSliderChanged(int value) const
{
    ui->unified_GraphicsView->setScaleFactor(std::clamp(value, 1, 500));
}

void QUnifiedPreview::onScaleFactorChanged(double scaleFactor) const
{
    ui->zoomFactor_Slider->setValue(qRound(scaleFactor * 100));
}

void QUnifiedPreview::setOriginalPreview(const ImagePreview& op)
{
    this->originalPreview = op;
    ui->unified_GraphicsView->addOriginalPixmap(op.image);
}

void QUnifiedPreview::setCompressedPreview(const ImagePreview& cp)
{
    this->compressedPreview = cp;
    ui->unified_GraphicsView->addPreviewPixmap(cp.image);
}

void QUnifiedPreview::swap() const
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
        ui->previewInfo_Label->setText(QString("ORIGINAL - %1 %2").arg(toHumanSize(static_cast<double>(this->originalPreview.fileInfo.size())), this->originalPreview.format));
    } else if (item == PreviewImageItem::PREVIEW) {
        ui->previewInfo_Label->setText(QString("PREVIEW - %1 %2").arg(toHumanSize(static_cast<double>(this->compressedPreview.fileInfo.size())), this->compressedPreview.format));
    } else {
        ui->previewInfo_Label->clear();
    }
}
