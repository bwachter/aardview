import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtMultimedia

ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 768
    title: context.currentTitle

    readonly property bool isMobile: width < 800 || Qt.platform.os === "android" || Qt.platform.os === "ios"

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: "☰"
                font.pixelSize: 20
                onClicked: drawerItem.open()
            }
            Label {
                text: context.currentTitle
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            ToolButton {
                text: "⚙"
                font.pixelSize: 16
                onClicked: context.openSettings()
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Open File")
        onAccepted: {
            let file = fileDialog.selectedFile.toString()
            if (file.startsWith("file://"))
                file = file.substring(7)
            context.loadFile(file)
        }
    }

    Drawer {
        id: drawerItem
        width: Math.min(root.width * 0.85, 420)
        height: root.height
        modal: isMobile
        interactive: isMobile || drawerItem.position < 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 4

            Label {
                text: qsTr("Current: %1").arg(context.currentPath)
                font.bold: true
                font.pixelSize: 12
                elide: Text.ElideMiddle
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("⬆ Parent Directory")
                Layout.fillWidth: true
                onClicked: {
                    let dir = context.currentPath
                    let parentDir = dir.substring(0, dir.lastIndexOf('/'))
                    if (parentDir && parentDir !== dir)
                        context.loadFile(parentDir)
                }
            }

            Label {
                text: qsTr("Directory Contents")
                font.bold: true
                font.pixelSize: 14
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                ListView {
                    id: dirList
                    model: context.dirContentsModel
                    boundsBehavior: Flickable.StopAtBounds

                    delegate: ItemDelegate {
                        width: dirList.width
                        text: (context.isDir(model.display) ? "📁 " : "🖼 ") + context.fileName(model.display)
                        highlighted: model.display === context.currentPath

                        onClicked: {
                            context.loadFile(model.display)
                            if (isMobile) drawerItem.close()
                        }
                    }
                }
            }

            Label {
                text: qsTr("Thumbnails")
                font.bold: true
                font.pixelSize: 14
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height * 0.35
                clip: true

                GridView {
                    id: thumbnailGrid
                    cellWidth: 136
                    cellHeight: 152
                    boundsBehavior: Flickable.StopAtBounds
                    model: context.tnViewModelProxy

                    delegate: Item {
                        width: thumbnailGrid.cellWidth
                        height: thumbnailGrid.cellHeight

                        Column {
                            anchors.centerIn: parent
                            spacing: 2

                            Image {
                                id: thumbImg
                                source: {
                                    let path = context.tnFilePath(index)
                                    return path ? "image://aardview/" + encodeURIComponent(path) : ""
                                }
                                width: 128
                                height: 128
                                fillMode: Image.PreserveAspectFit
                                anchors.horizontalCenter: parent.horizontalCenter
                                asynchronous: true
                                cache: false
                            }

                            Text {
                                text: model.fileName || model.display || ""
                                width: 128
                                elide: Text.ElideRight
                                horizontalAlignment: Text.AlignHCenter
                                font.pixelSize: 10
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                context.loadFile(context.tnFilePath(index))
                                if (isMobile) drawerItem.close()
                            }
                        }
                    }
                }
            }
        }
    }

    ViewerPage {
        id: viewerPage
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent
        z: -1
        property real startX: 0
        property real startY: 0

        onPressed: {
            startX = mouseX
            startY = mouseY
        }
        onReleased: {
            let dx = mouseX - startX
            let dy = mouseY - startY
            if (Math.abs(dx) > Math.abs(dy) && Math.abs(dx) > width * 0.1) {
                if (dx > 0) context.selectPrev()
                else context.selectNext()
            }
        }
    }

    Shortcut { sequence: "Ctrl+D"; onActivated: drawerItem.open() }
    Shortcut { sequence: "Space"; onActivated: context.videoMode ? toggleVideoPlay() : context.selectNext() }
    Shortcut { sequence: "Right"; onActivated: context.videoMode ? seekVideo(10000) : context.selectNext() }
    Shortcut { sequence: "Left"; onActivated: context.videoMode ? seekVideo(-10000) : context.selectPrev() }
    Shortcut { sequence: "Up"; onActivated: if (context.videoMode) viewerPage.adjustVolume(0.1) }
    Shortcut { sequence: "Down"; onActivated: if (context.videoMode) viewerPage.adjustVolume(-0.1) }
    Shortcut { sequence: "N"; onActivated: if (!context.videoMode) viewerPage.resetZoom() }
    Shortcut { sequence: "Z"; onActivated: if (!context.videoMode) viewerPage.toggleFitToWindow() }
    Shortcut { sequence: "Plus"; onActivated: if (!context.videoMode) viewerPage.zoomIn() }
    Shortcut { sequence: "Minus"; onActivated: if (!context.videoMode) viewerPage.zoomOut() }
    Shortcut { sequence: "R"; onActivated: if (context.videoMode) viewerPage.rotateVideo() }

    function toggleVideoPlay() {
        if (viewerPage.video.playbackState === MediaPlayer.PlayingState)
            viewerPage.video.pause()
        else
            viewerPage.video.play()
    }

    function seekVideo(offset) {
        viewerPage.video.position = Math.max(0, Math.min(viewerPage.video.duration,
            viewerPage.video.position + offset))
    }

    Component.onCompleted: {
        if (!isMobile)
            drawerItem.open()
    }
}
