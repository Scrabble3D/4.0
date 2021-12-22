import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Qt3D.Core
import Qt3D.Render
import Qt3D.Input
import Qt3D.Extras
import QtQuick.Scene3D

Item {
    anchors.fill: parent
    Scene3D
    {
        id: scrabbleCube
        focus: true
        aspects: ["input"]
        anchors.fill: parent
        visible: GamePlay.is3D

        Entity {
            id: sceneRoot

            Camera {
                id: mainCamera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: 16/9
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( 45.0, 25.0, -60.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            }

            OrbitCameraController {
                camera: mainCamera
            }

            MouseDevice {
              id: mouseDevice

            }
            MouseHandler {
                sourceDevice: mouseDevice

                property point lastPos
                property real pan
                property real tilt
                property real zoom
                property real wheelpos
                onPanChanged: mainCamera.panAboutViewCenter(pan);
                onTiltChanged: mainCamera.tiltAboutViewCenter(tilt);

                onPressed:
                    lastPos = Qt.point(mouse.x, mouse.y)
                onPositionChanged:
                    if (mouse.buttons === 1){
                        pan = -(mouse.x - lastPos.x)
                        tilt = (mouse.y - lastPos.y)
                        lastPos = Qt.point(mouse.x, mouse.y);
                    }
                onWheel: {
                    if (wheel.modifiers & Qt.ControlModifier) {
                        wheelpos += wheel.angleDelta.y/250 //TODO: better stepping
                        if (wheelpos < -1) {
                           GamePlay.activePosition -= 1
                           wheelpos = 0
                        } else
                        if (wheelpos > 1) {
                           GamePlay.activePosition += 1
                           wheelpos = 0
                        }
                    } else
                    {
                    zoom = wheel.angleDelta.y*0.01
                    if ((mainCamera.position.z+zoom<-5) &&
                        (mainCamera.position.z+zoom>-100))
                        mainCamera.translate(Qt.vector3d(0, 0, zoom), Camera.DontTranslateViewCenter)
                    }
                 }
            }
/*
            Layer {
                id: topLayer
                recursive: true
            }
*/
            components: [
                RenderSettings {
                    activeFrameGraph: SortPolicy {
                        sortTypes: [ SortPolicy.BackToFront ]
                    RenderStateSet {
                        renderStates: [
                            CullFace { mode: CullFace.NoCulling }
                        ]
                        ForwardRenderer {
                            camera: mainCamera
                            clearColor: SystemPalette.window
                        }
                    }
                    }
                },
/*                RenderSettings {
                    Viewport {
                        normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)
                        RenderSurfaceSelector {
                            CameraSelector {
                                id: cameraSelector
                                camera: mainCamera
                                FrustumCulling {
                                    ClearBuffers {
                                        buffers: ClearBuffers.AllBuffers
                                        clearColor: SystemPalette.window
                                        NoDraw {}
                                    }
                                    LayerFilter {
                                        filterMode: LayerFilter.DiscardAnyMatchingLayers
                                        layers: [topLayer]
                                    }
                                    LayerFilter {
                                        filterMode: LayerFilter.AcceptAnyMatchingLayers
                                        layers: [topLayer]
                                        ClearBuffers {
                                            buffers: ClearBuffers.DepthBuffer
                                        }
                                    }
                                }
                            }
                        }
                    }
                },
*/                InputSettings {}
            ]

            Entity {

                property int bs: GamePlay.boardSize
                property real pos: GamePlay.activePosition - GamePlay.boardSize/2
                components: [
                    PlaneMesh {
                        height: GamePlay.boardSize+4
                        width: GamePlay.boardSize+4
                    },
                    Transform {
                        rotation:
                            GamePlay.activeDimension === 0 ?
                                fromAxisAndAngle(0, 1, 0, 90)
                          : GamePlay.activeDimension === 1 ?
                                fromAxisAndAngle(1, 0, 0, 90)
                          :     fromAxisAndAngle(0, 0, 1, 90)
                        translation:
                            GamePlay.activeDimension === 0 ?
                                Qt.vector3d(-0.5, GamePlay.activePosition - GamePlay.boardSize/2, -0.5)
                          : GamePlay.activeDimension === 1 ?
                                Qt.vector3d(-0.5, -0.5, GamePlay.activePosition - GamePlay.boardSize/2)
                          :     Qt.vector3d(GamePlay.activePosition - GamePlay.boardSize/2, -0.5, -0.5)
                    },
                    PhongAlphaMaterial {
                        ambient: "black"
                        specular: "transparent"
                    }
                ]
            }


            NodeInstantiator {

                model: GamePlay.cubeModel
                delegate: Entity {
                    property int bs: GamePlay.boardSize
                    property int ft: fieldtype
                    property real x: (index % bs) - bs/2
                    property real y: Math.floor(index / bs) % bs - bs/2
                    property real z: Math.floor(index / (bs * bs)) - bs/2
                    components: [
                        CuboidMesh {
                            xExtent: 1
                            yExtent: 1
                            zExtent: 1
                        },
                        Transform {
                            translation: Qt.vector3d(x, y, z)
                        },
                        PhongAlphaMaterial {
                            ambient: config.colors.get(ft).itemColor
                            diffuse: config.colors.get(ft).itemColor
                            alpha: (ft === 1) ? 0.25 : 0.95
//                            specular: Qt.rgba(0, 0, 0, 0 )
                        }
                    ]
                }
            }
        }
    }
    RowLayout {
        id: topPanel
        visible: GamePlay.is3D

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 50
        Label {
            Layout.leftMargin: 8
            text: qsTr("Pos: ")
        }
        Slider {
            id: pos3D
            implicitWidth: 100
            from: 0
            to: GamePlay.boardSize-1
            value: GamePlay.activePosition
            onMoved: GamePlay.activePosition = value
            snapMode: Slider.SnapAlways
            stepSize: 1
        }
        Label {
            text: pos3D.value
        }
        Label {
            Layout.leftMargin: 16
            text: qsTr("Dim: ")
        }
        Rectangle {
            height: 38
            width: 38
            color: "lightgrey"
            border.width: GamePlay.activeDimension === 0 ? 3 : 0
            Image {
                anchors.fill: parent
                anchors.margins: 6
                source: "qrc:///resources/abscissa.png"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: GamePlay.activeDimension = 0
            }
        }
        Rectangle {
            height: 38
            width: 38
            color: "lightgrey"
            border.width: GamePlay.activeDimension === 1 ? 3 : 0
            Image {
                anchors.fill: parent
                anchors.margins: 6
                source: "qrc:///resources/ordinate.png"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: GamePlay.activeDimension = 1
            }
        }
        Rectangle {
            height: 38
            width: 38
            color: "lightgrey"
            border.width: GamePlay.activeDimension === 2 ? 3 : 0
            Image {
                anchors.fill: parent
                anchors.margins: 6
                source: "qrc:///resources/applicate.png"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: GamePlay.activeDimension = 2
            }
        }
        Item {
            Layout.fillWidth: true
        }
    }
    Image {
        id: showTopPanel
        visible: GamePlay.is3D

        width: 18
        height: 18
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 16
        anchors.topMargin: 16
        source: "qrc:///resources/up.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                topPanel.visible = !topPanel.visible;
                if (topPanel.visible)
                    parent.source = "qrc:///resources/up.png"
                else
                    parent.source = "qrc:///resources/down.png"
            }
        }
    }
}
