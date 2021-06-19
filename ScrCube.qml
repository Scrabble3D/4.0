import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QtQuick3D
import QtDataVisualization
//import QtQuick3D.Helpers

Page {
    id: scrCube

    View3D {
        id: view
        anchors.fill: parent

        environment: SceneEnvironment {
            id: seEnvironment
            clearColor: "skyblue"
            backgroundMode: SceneEnvironment.Color
        }

        PerspectiveCamera {
            id: pcCamera
                position: Qt.vector3d(0, 0, -10)
            //eulerRotation.x: -30
            z: -10
        }


        DirectionalLight {
            eulerRotation.x: -30
            eulerRotation.y: -70
        }

        Repeater3D {
            id: rep3d
            model: config.is3D ? config.fieldCount * config.fieldCount * config.fieldCount : 0
            Model {
                property int x: (index % config.fieldCount)
                property int y: Math.floor(index / (config.fieldCount * config.fieldCount))
                property int z: (Math.floor(index / config.fieldCount) % config.fieldCount)
                property int aFieldType: config.fieldType[ x + y*config.fieldCount + z*config.fieldCount*config.fieldCount ]
                source: "#Cube"
                position: Qt.vector3d(x, y, z)
                scale: Qt.vector3d(0.01, 0.01, 0.01)
                materials: [
                    PrincipledMaterial {
                        baseColor: config.colors.get(aFieldType).itemColor
                        opacity: (aFieldType === 1) ? 0.05 : 0.75
                    }
                 ]
            }
        }


        MouseArea {
             id: inputArea
             anchors.fill: parent
             hoverEnabled: true
             acceptedButtons: Qt.LeftButton | Qt.RightButton
             property int mouseX: config.is3D ? pcCamera.eulerrotation.x : 0
             property int mouseY: config.is3D ? pcCamera.eulerrotation.y : 0

             onPositionChanged: {
                 if ((mouseX > -1) && (pressedButtons === Qt.LeftButton)) {
                    //pcCamera.rotation.y -= mouse.y+mouseY
                    pcCamera.eulerRotation.y -= mouse.x-mouseX
                    //pcCamera.rotation.x -= mouse.x+mouseX
                    pcCamera.eulerRotation.x -= mouse.y-mouseY
                 }
//                 pcCamera.lookAt(Qt.vector3d(config.fieldCount/2,config.fieldCount/2,config.fieldCount/2))
                 mouseX = mouse.x
                 mouseY = mouse.y
             }

             onWheel: {
                 var fZoom = pcCamera.position.z
/*                 if (fZoom > 100)
                     fZoom += wheel.angleDelta.y / 2.0;
                 else if (fZoom > 50)
                     fZoom += wheel.angleDelta.y / 60.0;
                 else
                     fZoom += wheel.angleDelta.y / 120.0;
                 if (fZoom > 1000)
                     fZoom = 1000;
                 else if (fZoom < 10)
                     fZoom = 10;
*/
                 fZoom += wheel.angleDelta.y / 10
                 pcCamera.position.z = fZoom
                 console.log(fZoom)
//                 pcCamera.zoomLevel = fZoom
//                 pcCamera.lookAt(Qt.vector3d(config.fieldCount/2,config.fieldCount/2,config.fieldCount/2))
//                 pcCamera.lookAt(Qt.vector3d(0,0,0))
             }
         }
    }

}
