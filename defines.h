#pragma once

#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

//TODO: defines: make path configurable
// portable
// QDir dictionaryPath() {return QCoreApplication::applicationDirPath(); }

// desktop
static const QDir dictionaryPath() { return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation); }
// /home/<user>.config/Scrabble3D
static const QDir appPath() { return QCoreApplication::applicationDirPath(); }
// .<exe>
static const QDir configPath() { return dictionaryPath(); }
//
static const QDir docPath() { return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); }

static const QDir tempPath() { return QStandardPaths::writableLocation(QStandardPaths::TempLocation); }
// mobile
/*
static QDir dictionaryPath() {return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); }
static QDir configPath() {return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); }
*/
