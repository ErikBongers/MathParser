var Calc = {
    addButton: function(label, color) {
        let button = document.createElement('div');
        button.id = 'todo';
        button.className = 'grid-item ' + color;
        button.innerHTML = label;
        button.setAttribute('onclick','Calc.typeIt(event)');

        let calcCont = document.querySelector('#calculator');
        calcCont.appendChild(button);
        },
    addButtons: function()
        {
            Calc.addButton("7", "light");
            Calc.addButton("8", "light");
            Calc.addButton("9", "light");
            Calc.addButton("/", "medium");
            Calc.addButton("(", "medium");
            Calc.addButton("4", "light");
            Calc.addButton("5", "light");
            Calc.addButton("6", "light");
            Calc.addButton("*", "medium");
            Calc.addButton(")", "medium");
            Calc.addButton("1", "light");
            Calc.addButton("2", "light");
            Calc.addButton("3", "light");
            Calc.addButton("-", "medium");
            Calc.addButton(";", "medium");
            Calc.addButton(".", "medium");
            Calc.addButton("0", "light");
            Calc.addButton("=", "medium");
            Calc.addButton("+", "medium");
            Calc.addButton("^", "medium");
        },
    typeIt: function(event){
        let theText = event.srcElement.innerHTML;
        let txtInput = document.querySelector("#txtInput");
        if (txtInput.selectionStart || txtInput.selectionStart == '0') {
            var startPos = txtInput.selectionStart;
            var endPos = txtInput.selectionEnd;
            txtInput.value = txtInput.value.substring(0, startPos)
                + theText
                + txtInput.value.substring(endPos, txtInput.value.length);
                txtInput.selectionStart = startPos + theText.length; txtInput.selectionEnd = startPos + theText.length;
        } else {
            txtInput.value += theText;
        }
        Module.parseAfterChange();
        txtInput.focus();
    } 
};

Calc.addButtons();

