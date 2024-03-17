#include "AdvancedImportDialog.h"
#include "ui_AdvancedImportDialog.h"

#include <QDirIterator>
#include <QFileDialog>
#include <QMenu>
#include <QSettings>
#include <QStandardPaths>
#include <utils/Utils.h>

AdvancedImportDialog::AdvancedImportDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::AdvancedImportDialog)
{
    ui->setupUi(this);

    this->addButtonContextMenu = new QMenu();
    this->addFilesAction = new QAction(tr("Add files..."));
    this->addFolderAction = new QAction(tr("Add folder..."));
    this->importFromListAction = new QAction(tr("Import from list..."));

    this->setupAddButtonMenu();

    this->loadPreferences();
    this->setupConnections();
}

AdvancedImportDialog::~AdvancedImportDialog()
{
    delete ui;
}

void AdvancedImportDialog::accept()
{
    QStringList fileList;
    for (int i = 0; i < ui->importList_ListWidget->count(); ++i) {
        QFileInfo fileInfo(ui->importList_ListWidget->item(i)->text());
        QString absoluteFilePath = fileInfo.absoluteFilePath();
        if (fileInfo.isFile()) {
            if (!passesFilters(fileInfo)) {
                continue;
            }
            fileList << absoluteFilePath;
        } else if (fileInfo.isDir()) {
            fileList << scanDirectoryWithFilters(absoluteFilePath, ui->importSubfolders_CheckBox->isChecked());
        }
    }

    QDialog::accept();

    emit importTriggered(fileList);
}

void AdvancedImportDialog::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void AdvancedImportDialog::setupAddButtonMenu() const
{
    this->addButtonContextMenu->addAction(this->addFilesAction);
    this->addButtonContextMenu->addAction(this->addFolderAction);
    this->addButtonContextMenu->addAction(this->importFromListAction);

    ui->add_Button->setMenu(this->addButtonContextMenu);
}

void AdvancedImportDialog::loadPreferences()
{
    ui->importSubfolders_CheckBox->setChecked(QSettings().value("preferences/advaced_import/import_subfolders", "").toBool());
    ui->skipBySize_CheckBox->setChecked(QSettings().value("preferences/advaced_import/skip_by_size/enabled", QSettings().value("preferences/general/skip_by_size/enabled", false).toBool()).toBool());
    ui->skipBySizeCondition_ComboBox->setCurrentIndex(QSettings().value("preferences/advaced_import/skip_by_size/condition", QSettings().value("preferences/general/skip_by_size/condition", 0).toInt()).toInt());
    ui->skipBySizeSize_SpinBox->setValue(QSettings().value("preferences/advaced_import/skip_by_size/value", QSettings().value("preferences/general/skip_by_size/value", 500).toInt()).toInt());
    ui->skipBySizeUnit_ComboBox->setCurrentIndex(QSettings().value("preferences/advaced_import/skip_by_size/unit", QSettings().value("preferences/general/skip_by_size/unit", 1).toInt()).toInt());
    ui->filenamePattern_LineEdit->setText(QSettings().value("preferences/advaced_import/filename_pattern", "").toString());

    this->lastOpenedDirectory = QSettings().value("extra/last_opened_directory", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first()).toString();
}

void AdvancedImportDialog::setupConnections()
{
    connect(this->addFilesAction, &QAction::triggered, this, &AdvancedImportDialog::onAddFilesActionTriggered);
    connect(this->addFolderAction, &QAction::triggered, this, &AdvancedImportDialog::onAddFolderActionTriggered);
    connect(this->importFromListAction, &QAction::triggered, this, &AdvancedImportDialog::onImportFromListActionTriggered);

    connect(ui->remove_Button, &QToolButton::clicked, this, &AdvancedImportDialog::onRemoveButtonClicked);
    connect(ui->importSubfolders_CheckBox, &QCheckBox::toggled, this, &AdvancedImportDialog::onImportSubfoldersToggled);
    connect(ui->skipBySize_CheckBox, &QCheckBox::toggled, this, &AdvancedImportDialog::onSkipBySizeToggled);
    connect(ui->skipBySizeCondition_ComboBox, &QComboBox::currentIndexChanged, this, &AdvancedImportDialog::onSkipBySizeConditionChanged);
    connect(ui->skipBySizeSize_SpinBox, &QSpinBox::valueChanged, this, &AdvancedImportDialog::onSkipBySizeValueChanged);
    connect(ui->skipBySizeUnit_ComboBox, &QComboBox::currentIndexChanged, this, &AdvancedImportDialog::onSkipBySizeUnitChanged);
    connect(ui->filenamePattern_LineEdit, &QLineEdit::textChanged, this, &AdvancedImportDialog::onFilenamePatternTextChanged);
}

void AdvancedImportDialog::setLastOpenedDirectory(const QString& directory)
{
    this->lastOpenedDirectory = directory;
    QSettings().setValue("extra/last_opened_directory", directory);
}

bool AdvancedImportDialog::passesFilters(const QFileInfo& fileInfo) const
{
    if (ui->skipBySize_CheckBox->isChecked()) {
        // TODO Make it an Enum
        int unit = ui->skipBySizeUnit_ComboBox->currentIndex();
        int condition = ui->skipBySizeCondition_ComboBox->currentIndex();
        int size = ui->skipBySizeSize_SpinBox->value() << (unit * 10);
        size_t imageSize = fileInfo.size();
        if ((condition == 0 && imageSize > size) || (condition == 1 && imageSize == size) || (condition == 2 && imageSize < size)) {
            return false;
        }
    }

    if (!ui->filenamePattern_LineEdit->text().isEmpty()) {
        QString filename = fileInfo.fileName();

        QString pattern = ui->filenamePattern_LineEdit->text();
        QRegularExpression regex(pattern);

        if (!regex.match(filename).hasMatch()) {
            return false;
        }
    }

    return true;
}

void AdvancedImportDialog::onAddFilesActionTriggered()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this,
        tr("Import files..."),
        this->lastOpenedDirectory,
        QIODevice::tr("Image Files") + " (*.jpg *.jpeg *.png *.webp *.tif *.tiff)");

    if (fileList.isEmpty()) {
        return;
    }

    this->setLastOpenedDirectory(QFileInfo(fileList.first()).absolutePath());
    ui->importList_ListWidget->addItems(fileList);
}

void AdvancedImportDialog::onAddFolderActionTriggered()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this, tr("Import folder..."),
        this->lastOpenedDirectory,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (directoryPath.isEmpty()) {
        return;
    }

    this->setLastOpenedDirectory(directoryPath);
    ui->importList_ListWidget->addItems(QStringList() << directoryPath);
}

void AdvancedImportDialog::onImportFromListActionTriggered()
{
    auto listFilePath = QFileDialog::getOpenFileName(this, tr("Import file..."), this->lastOpenedDirectory);

    if (listFilePath.isEmpty()) {
        return;
    }

    this->setLastOpenedDirectory(listFilePath);

    QFile listFile(listFilePath);
    if (!listFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QStringList fileList;
    while (!listFile.atEnd()) {
        auto pathToAdd = listFile.readLine().trimmed();
        if (!QFileInfo::exists(pathToAdd)) {
            continue;
        }
        fileList.append(pathToAdd);
    }

    ui->importList_ListWidget->addItems(fileList);
}

void AdvancedImportDialog::onRemoveButtonClicked() const
{
    QList<QListWidgetItem*> selectedItems = ui->importList_ListWidget->selectedItems();
    while (!selectedItems.isEmpty()) {
        delete ui->importList_ListWidget->takeItem(ui->importList_ListWidget->row(selectedItems.takeFirst()));
    }
}

void AdvancedImportDialog::onImportSubfoldersToggled(bool checked)
{
    QSettings().setValue("preferences/advaced_import/import_subfolders", checked);
}

void AdvancedImportDialog::onSkipBySizeToggled(bool checked)
{
    QSettings().setValue("preferences/advaced_import/skip_by_size/enabled", checked);
}

void AdvancedImportDialog::onSkipBySizeConditionChanged(int index)
{
    QSettings().setValue("preferences/advaced_import/skip_by_size/condition", index);
}

void AdvancedImportDialog::onSkipBySizeValueChanged(int value)
{
    QSettings().setValue("preferences/advaced_import/skip_by_size/value", value);
}

void AdvancedImportDialog::onSkipBySizeUnitChanged(int index)
{
    QSettings().setValue("preferences/advaced_import/skip_by_size/unit", index);
}

void AdvancedImportDialog::onFilenamePatternTextChanged(const QString& text)
{
    QSettings().setValue("preferences/advanced_import/filename_pattern", text);
}

QStringList AdvancedImportDialog::scanDirectoryWithFilters(const QString& directory, bool subfolders) const
{
    QStringList inputFilterList = { "*.jpg", "*.jpeg", "*.png", "*.webp", "*.tif", "*.tiff" };
    QStringList fileList = {};
    auto iteratorFlags = subfolders ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    // Collecting all files in folder
    if (QDir(directory).exists()) {
        QDirIterator it(directory,
            inputFilterList,
            QDir::AllEntries,
            iteratorFlags);

        while (it.hasNext()) {
            it.next();
            QString filePath = it.filePath();
            if (passesFilters(QFileInfo(filePath))) {
                fileList.append(filePath);
            }
        }
    }

    return fileList;
}