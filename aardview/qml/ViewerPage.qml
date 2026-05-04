import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Item {
    id: viewerPage

    property alias video: videoPlayer
    property alias audioOutput: audioOut

    property real scaleFactor: 1.0
    property bool fitToWindow: true
    property real videoRotation: 0

    Connections {
        target: context
        function onRequestLoadVideo(path) {
            videoPlayer.source = Qt.resolvedUrl("file://" + path)
            videoPlayer.play()
        }
    }

    MediaPlayer {
        id: videoPlayer
        audioOutput: audioOut
        videoOutput: videoOutput
        onPlaybackStateChanged: {
            if (playbackState === MediaPlayer.StoppedState && position >= duration - 100)
                onVideoEnd()
        }
    }

    AudioOutput {
        id: audioOut
        volume: 1.0
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        visible: context.videoMode
        fillMode: VideoOutput.PreserveAspectFit
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        visible: !context.videoMode
        contentWidth: imageContainer.width * scaleFactor
        contentHeight: imageContainer.height * scaleFactor
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        Image {
            id: imageContainer
            source: context.displayPath ? "image://aardview/" + encodeURIComponent(context.displayPath) : ""
            width: fitToWindow ? flickable.width : implicitWidth
            height: fitToWindow ? flickable.height : implicitHeight
            fillMode: fitToWindow ? Image.PreserveAspectFit : Image.Pad
            anchors.centerIn: parent
            asynchronous: true
            smooth: true

            onStatusChanged: {
                if (status === Image.Ready) {
                    scaleFactor = 1.0
                }
            }
        }
    }

    PinchArea {
        anchors.fill: parent
        enabled: !context.videoMode

        property real initialScale: 1.0

        onPinchStarted: {
            initialScale = scaleFactor
        }

        onPinchUpdated: {
            scaleFactor = Math.max(0.1, Math.min(10.0, initialScale * pinch.scale))
        }
    }

    function zoomIn() {
        scaleFactor *= 1.25
    }

    function zoomOut() {
        scaleFactor *= 0.8
    }

    function resetZoom() {
        scaleFactor = 1.0
    }

    function toggleFitToWindow() {
        fitToWindow = !fitToWindow
        if (fitToWindow) scaleFactor = 1.0
    }

    function rotateVideo() {
        videoRotation = (videoRotation + 90) % 360
        videoOutput.rotation = videoRotation
    }

    function adjustVolume(delta) {
        audioOut.volume = Math.max(0.0, Math.min(1.0, audioOut.volume + delta))
    }

    function onVideoEnd() {
        let action = context.videoEndAction
        switch(action) {
        case 0: // Stop
            videoPlayer.stop()
            break
        case 1: // Loop
            videoPlayer.position = 0
            videoPlayer.play()
            break
        case 2: // Jump next
            context.selectNext()
            break
        }
    }
}
