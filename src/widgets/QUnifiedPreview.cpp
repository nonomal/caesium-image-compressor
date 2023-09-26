#include "QUnifiedPreview.h"
#include "ui_QUnifiedPreview.h"

QUnifiedPreview::QUnifiedPreview(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::QUnifiedPreview)
{
    ui->setupUi(this);

    ui->swap_ToolButton->setIcon(QIcon(":/icons/ui/swap.svg"));
    ui->zoomFit_ToolButton->setIcon(QIcon(":/icons/ui/zoom_fit.svg"));

    connect(ui->zoomFactor_Slider, &QSlider::valueChanged, this, &QUnifiedPreview::onZoomSliderChanged);
    connect(ui->unified_GraphicsView, &QUnifiedGraphicsView::scaleFactorChanged, this, &QUnifiedPreview::onScaleFactorChanged);
    connect(ui->swap_ToolButton, &QToolButton::pressed, ui->unified_GraphicsView, &QUnifiedGraphicsView::swap);
    connect(ui->zoomFull_ToolButton, &QToolButton::pressed, ui->unified_GraphicsView, &QUnifiedGraphicsView::zoomToFull);
    connect(ui->zoomFit_ToolButton, &QToolButton::pressed, ui->unified_GraphicsView, &QUnifiedGraphicsView::zoomToFit);
}

QUnifiedPreview::~QUnifiedPreview()
{
    delete ui;
}

void QUnifiedPreview::addOriginalPixmap(const QPixmap& pixmap)
{
    ui->unified_GraphicsView->addOriginalPixmap(pixmap);
}

void QUnifiedPreview::addPreviewPixmap(const QPixmap& pixmap)
{
    ui->unified_GraphicsView->addPreviewPixmap(pixmap);
}

void QUnifiedPreview::clear()
{
    ui->unified_GraphicsView->clearPixmaps();
    ui->unified_GraphicsView->resetScaleFactor();
}

void QUnifiedPreview::setLoading(bool loading)
{
    ui->unified_GraphicsView->setLoading(loading);
}

void QUnifiedPreview::onZoomSliderChanged(int value) {
    double factor = std::clamp(value, 1, 500);
    ui->unified_GraphicsView->setScaleFactor((double)factor);
}

void QUnifiedPreview::onScaleFactorChanged(double scaleFactor)
{
    ui->zoomFactor_Slider->setValue(qRound(scaleFactor * 100));
}
