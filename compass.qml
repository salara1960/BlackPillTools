import QtQuick 2.0


Item {

    property real az: 0
    property real last_az: 0

    Image {
        source: "compass.svg"
    }

    Image {
            x: 189
            y: 70
            width: 32
            height: 200
            source: "arrow.svg"
            transform: Rotation {
                id : vector
                origin.x: 16
                origin.y: 100
                angle: az
            }
    }

    Timer {
        interval: 50; running: true; repeat: true;
        onTriggered: {
            az = area.get_az();
        }
    }

}

