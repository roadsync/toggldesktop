// Copyright 2014 Toggl Desktop developers.

#include "./timerwidget.h"
#include "./ui_timerwidget.h"

#include <QApplication>  // NOLINT
#include <QCompleter>  // NOLINT

#include "./autocompletelistmodel.h"
#include "./autocompleteview.h"
#include "./timeentryview.h"
#include "./toggl.h"

TimerWidget::TimerWidget(QWidget *parent) : QFrame(parent),
ui(new Ui::TimerWidget),
timer(new QTimer(this)),
duration(0),
project(""),
tagsHolder(""),
timeEntryAutocompleteNeedsUpdate(false),
descriptionModel(new AutocompleteListModel(this)),
selectedTaskId(0),
selectedProjectId(0) {
    ui->setupUi(this);

    connect(TogglApi::instance, SIGNAL(displayStoppedTimerState()),
            this, SLOT(displayStoppedTimerState()));

    connect(TogglApi::instance, SIGNAL(displayRunningTimerState(TimeEntryView*)),  // NOLINT
            this, SLOT(displayRunningTimerState(TimeEntryView*)));  // NOLINT

    connect(TogglApi::instance, SIGNAL(displayMinitimerAutocomplete(QVector<AutocompleteView*>)),  // NOLINT
            this, SLOT(displayMinitimerAutocomplete(QVector<AutocompleteView*>)));  // NOLINT

    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
            this, SLOT(focusChanged(QWidget*, QWidget*)));

    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));

    connect(ui->description, SIGNAL(returnPressed()),
            this, SLOT(descriptionReturnPressed()));
    connect(ui->description, SIGNAL(projectSelected(QString,uint64_t,QString,QString,uint64_t)),
            this, SLOT(descriptionProjectSelected(QString,uint64_t,QString,QString,uint64_t)));
    connect(ui->description, SIGNAL(billableChanged(bool)),
            this, SLOT(descriptionBillableChanged(bool)));
    connect(ui->description, SIGNAL(tagsChanged(QString)),
            this, SLOT(descriptionTagsChanged(QString)));

    connect(ui->deleteProject, &QPushButton::clicked, this, &TimerWidget::clearProject);
    connect(ui->deleteTask, &QPushButton::clicked, this, &TimerWidget::clearTask);

    ui->description->setModel(descriptionModel);
    ui->taskFrame->setVisible(false);
    ui->projectFrame->setVisible(false);

    ui->billable->setVisible(false);
    ui->tags->setVisible(false);

    descriptionPlaceholder = "What are you doing?";
    tagsHolder = "";
}

TimerWidget::~TimerWidget() {
    timer->stop();

    delete ui;
}

void TimerWidget::descriptionReturnPressed() {
    start();
}

void TimerWidget::descriptionProjectSelected(const QString &projectName, uint64_t projectId, const QString &color, const QString &taskName, uint64_t taskId) {
    ui->projectFrame->setVisible(true);
    ui->project->setText(QString("<font color=\"%1\">%2</font>").arg(color).arg(projectName));
    selectedProjectId = projectId;
    clearTask();
    if (taskId && !taskName.isEmpty()) {
        ui->taskFrame->setVisible(true);
        ui->task->setText(QString("<font color=\"gray\">%1</font>").arg(taskName));
        selectedTaskId = taskId;
    }
}

void TimerWidget::descriptionBillableChanged(bool billable) {
    ui->billable->setVisible(billable);
}

void TimerWidget::descriptionTagsChanged(const QString &tags) {
    ui->tags->setVisible(!tags.isEmpty());
    ui->tags->setToolTip("<p style='color:white;background-color:black;'>" + tags + "</p>");
    tagsHolder = tags;
}

void TimerWidget::clearProject() {
    selectedProjectId = 0;
    if (guid.isEmpty()) {
        ui->project->clear();
        ui->projectFrame->setVisible(false);
    }
    // else branch API call happens in clearTask
    clearTask();
}

void TimerWidget::clearTask() {
    selectedTaskId = 0;
    if (guid.isEmpty()) {
        ui->task->clear();
        ui->taskFrame->setVisible(false);
    }
    else {
        TogglApi::instance->setTimeEntryProject(guid,
                                                selectedTaskId,
                                                selectedProjectId,
                                                "");
    }
}

void TimerWidget::focusChanged(QWidget *old, QWidget *now) {
    if (old == ui->description) {
        if (ui->description->currentText().length() == 0) {
            ui->description->setEditText(descriptionPlaceholder);
        }
        if (timeEntryAutocompleteNeedsUpdate) {
            displayMinitimerAutocomplete(timeEntryAutocompleteUpdate);
        }
    }
    if (now == ui->description &&
            ui->description->currentText() == descriptionPlaceholder) {
        ui->description->setEditText("");
    }
}

void TimerWidget::displayRunningTimerState(
    TimeEntryView *te) {
    guid = te->GUID;
    selectedTaskId = te->TID;
    selectedProjectId = te->PID;

    ui->start->setText("Stop");
    ui->start->setStyleSheet(
        "background-color: #e20000; color:'white'; font-weight: bold;");

    QString description = (te->Description.length() > 0) ?
                          te->Description : "(no description)";

    ui->description->setEditText(description);
    ui->description->setEnabled(false);

    ui->duration->setText(te->Duration);
    ui->duration->setEnabled(false);
    ui->duration->setToolTip(
        QString("<p style='color:white;background-color:black;'>Started: " +
                te->StartTimeString+"</p>"));

    if (!te->ProjectLabel.isEmpty()) {
        ui->projectFrame->setVisible(true);
        setEllipsisTextToLabel(ui->project, te->ProjectLabel);
    }
    else {
        ui->projectFrame->setVisible(false);
    }
    if (!te->TaskLabel.isEmpty()) {
        ui->taskFrame->setVisible(true);
        setEllipsisTextToLabel(ui->task, te->TaskLabel);
    }
    else {
        ui->taskFrame->setVisible(false);
    }

    ui->billable->setVisible(te->Billable);
    ui->tags->setVisible(!te->Tags.isEmpty());
    ui->tags->setToolTip(QString("<p style='color:white;background-color:black;'>" +
                                 te->Tags + "</p>"));

    duration = te->DurationInSeconds;

    if (te->Description.length() > 0) {
        ui->description->setToolTip(
            QString("<p style='color:white;background-color:black;'>" +
                    te->Description + "</p>"));
    }
    if (te->ProjectAndTaskLabel.length() > 0) {
        ui->project->setToolTip(
            QString("<p style='color:white;background-color:black;'>" +
                    te->ProjectAndTaskLabel+"</p>"));
    } else {
        ui->project->setToolTip(QString(""));
    }

    disconnect(this, SLOT(start()));
    disconnect(this, SLOT(stop()));

    connect(this, SIGNAL(buttonClicked()), this, SLOT(stop()));

    timer->start(1000);
}

void TimerWidget::displayStoppedTimerState() {
    guid = QString();
    selectedTaskId = 0;
    selectedProjectId = 0;

    ui->start->setText("Start");
    ui->start->setStyleSheet(
        "background-color: #47bc00; color:'white'; font-weight: bold;");

    if (!ui->description->hasFocus()) {
        ui->description->setEditText(descriptionPlaceholder);

        ui->project->setText("");
        ui->project->setToolTip(QString(""));
        ui->projectFrame->setVisible(false);

        ui->billable->setVisible(false);
        ui->tags->setVisible(false);

        tagsHolder = "";
    }

    ui->description->setEnabled(true);
    ui->description->setToolTip(QString(""));

    if (!ui->duration->hasFocus()) {
        ui->duration->setText("");
    }
    ui->duration->setEnabled(true);
    ui->duration->setToolTip(QString(""));

    duration = 0;

    disconnect(this, SLOT(start()));
    disconnect(this, SLOT(stop()));

    connect(this, SIGNAL(buttonClicked()), this, SLOT(start()));

    timer->stop();
}

void TimerWidget::on_start_clicked() {
    emit buttonClicked();
}

void TimerWidget::start() {
    /*
    QVariant data = ui->description->currentData();
    if (data.canConvert<AutocompleteView *>()) {
        AutocompleteView *view = data.value<AutocompleteView *>();
        task_id = view->TaskID;
        project_id = view->ProjectID;
    }
    */

    QString description = ui->description->currentText();
    if (description == descriptionPlaceholder) {
        description = "";
    }

    TogglApi::instance->start(description,
                              ui->duration->text(),
                              selectedTaskId,
                              selectedProjectId,
                              tagsHolder.toStdString().c_str(),
                              ui->billable->isVisible());
    tagsHolder = "";
}

void TimerWidget::stop() {
    TogglApi::instance->stop();
}

void TimerWidget::displayMinitimerAutocomplete(
    QVector<AutocompleteView *> list) {
    timeEntryAutocompleteUpdate = list;
    timeEntryAutocompleteNeedsUpdate = true;
    if (ui->description->hasFocus()) {
        return;
    }
    QString currentText = ui->description->currentText();
    ui->description->clear();
    descriptionModel->setList(list);
    timeEntryAutocompleteNeedsUpdate = false;
    ui->description->setEditText(currentText);
}

void TimerWidget::timeout() {
    if (!isVisible()) {
        return;
    }
    if (duration >= 0) {
        return;
    }
    ui->duration->setText(TogglApi::formatDurationInSecondsHHMMSS(duration));
}

void TimerWidget::mousePressEvent(QMouseEvent *event) {
    TogglApi::instance->editRunningTimeEntry("");
    QWidget::mousePressEvent(event);
}

void TimerWidget::on_duration_returnPressed() {
    start();
}

void TimerWidget::resizeEvent(QResizeEvent* event)
{
    setEllipsisTextToLabel(ui->project, project);
    QWidget::resizeEvent(event);
}

void TimerWidget::setEllipsisTextToLabel(QLabel *label, QString text)
{
    QFontMetrics metrix(label->font());
    int width = label->width() - 4;
    QString clippedText = metrix.elidedText(text, Qt::ElideRight, width);
    label->setText(clippedText);
}
