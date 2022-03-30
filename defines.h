#pragma once

#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

// portable
// QDir dictionaryPath() {return QCoreApplication::applicationDirPath(); }

// desktop
static const QDir dictionaryPath() { return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation); }
// /home/<user>.config/QScrabble
static const QDir appPath() { return QCoreApplication::applicationDirPath(); }
// .<exe>
static const QDir configPath() { return appPath(); }
//
static const QDir docPath() { return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); }

static const QDir tempPath() { return QStandardPaths::writableLocation(QStandardPaths::TempLocation); }
// mobile
/*
static QDir dictionaryPath() {return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); }
static QDir configPath() {return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); }
*/
