import QtQuick

Item {
    //property var fields: defaults.fieldsClassicScrabble
    property int boardSize: 15
//    property alias rack: rack
    property alias bag: bag
    property bool isGameRunning: false
    property int currentMove: 0
    property int lastError: 0
/*
    ListModel {
        id: rack
    }
*/
    ListModel {
        id: bag
    }
    ListModel {
        id: letters
    }
    ListModel {
        id: currentLetters
    }

    //bag holds all letters depending on language and letterset
    function fillBag() {
        for (var i = 0; i < config.letterSet.rowCount; i++) {
            var aLetter = config.letterSet.getRow(i)
            for (var j=0; j<aLetter.count; j++)
                bag.append({"letter":aLetter.letter,"value":aLetter.value,"isJoker":false})
        }
        for (i = 0; i < config.numberOfJokers; i++)
            bag.append({"letter":" ","value":0,"isJoker":true})

        //TODO: randseed, see http://davidbau.com/archives/2010/01/30/random_seeds_coded_hints_and_quintillions.html#more
        //Fisher–Yates shuffle
        for (i = bag.count-1; i > 0; i--) {
          j = Math.floor(Math.random() * (i + 1))
          bag.move(j,i,1)
        }
    }

    function nextPlayer() {
/*        for (var i = rack.count; i < config.numberOfLettersOnRack; i++) {
            var j = bag.count-1
            if (j > 0) {
                var aLetter = bag.get(j)
                rack.append(aLetter)
                bag.remove(j,1)
            }
        }
*/        currentMove++
        lastError = 1 //leNoLetter
    }

    function initLetters() {
        letters.clear()
        for (var i = 0; i < config.fields.length; i++)
            letters.append({"letter":String.fromCharCode(0),"value":0,"move":0,"position":0})
    }
/*
    function getLetter(index) {
        if (index < letters.count)
            return [letters.get(index).letter, letters.get(index).value]
        else
            return [String.fromCharCode(0), 0]
    }

    function setLetter(letter, value, move, position) {
        letters.get(position).letter = letter
        letters.get(position).value = value
        letters.get(position).move = move
    }
*/
    function startNewGame() {
        //TODO: tableview.model -> qvariantlist
        var letterlist = [];
        for (var i=0; i<config.letterSet.rowCount; i++) {
            letterlist.push(config.letterSet.getRow(i).letter);
            letterlist.push(config.letterSet.getRow(i).value);
            letterlist.push(config.letterSet.getRow(i).count);
        }

        GamePlay.startNewGame(["Heiko"],
                              config.numberOfLettersOnRack,
                              config.is3D,
                              config.fields,
                              letterlist, //config.letterSet,
                              true,
                              50,
                              7,
                              10,
                              true,
                              0,
                              10,
                              3,
                              false,
                              false,
                              true,
                              true,
                              0,
                              10,
                              true,
                              0,
                              30,
                              10,
                              10,
                              50,
                              false,
                              false);
        boardSize = 0 //to clear the board
        fields = config.fields.slice()
        boardSize = config.fieldCount
        bag.clear()
        fillBag()
//        rack.clear()
        currentLetters.clear()
        currentMove = 0
        initLetters()
        isGameRunning = true
        nextPlayer()
    }

    function checkNoLetter() {
        currentLetters.clear()
        for (var i = 0; i < letters.count; i++)
            if (letters.get(i).move === currentMove)
                currentLetters.append({"letter":letters.get(i).letter,"value":letters.get(i).value,"position":i})
        if (currentLetters.count === 0) {
            lastError = 1 //leNoLetter
            return false
        }
        return true
    }
    function checkFirstMove() {
        if (currentMove>1)
            return true
        for (var i = 0; i < currentLetters.count; i++)
            if (fields[currentLetters.get(i).position] === 0)
                return true;
        lastError = 2 //leFirstMove
        return false
    }
    function checkDimension() {
        lastError = 3 //leDimension
        var dimension = 0
        var x = Math.floor(currentLetters.get(0).position / boardSize)
        var y = currentLetters.get(0).position % boardSize
        console.log(currentLetters.get(0).letter+": " + Math.floor(currentLetters.get(0).position / boardSize) + "," + currentLetters.get(0).position % boardSize)
        for (var i = 1; i < currentLetters.count; i++) {
            console.log(currentLetters.get(i).letter+": " + Math.floor(currentLetters.get(i).position / boardSize) + "," + currentLetters.get(i).position % boardSize)
            if (Math.floor(currentLetters.get(i).position / boardSize) === x) {
                if (dimension === 0)
                    dimension = 1
            } else
            if (currentLetters.get(i).position % boardSize === y) {
                if (dimension === 0)
                    dimension = 2
            } else
                return false
        }
        lastError = 0
    }

    function checkMove() {
        lastError = 0 //leNoError
        if (checkNoLetter())
            if (checkFirstMove())
                if (checkDimension())
                    return true
    }
}
