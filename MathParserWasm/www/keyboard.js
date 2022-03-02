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
        let transactionSpec = cm.editor.state.replaceSelection(theText);
        let transaction = cm.editor.state.update(transactionSpec);
        cm.editor.update([transaction]);
        cm.editor.focus();
    } 
};

Calc.addButtons();

