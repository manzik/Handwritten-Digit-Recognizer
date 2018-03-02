<<<<<<< HEAD
﻿var lang = "en";
var mynet;
=======
var lang = "en";
>>>>>>> de92a2fd9ad65aa0ed16d03acc1cb3242ea04718
function langclick()
{
    if (lang == "en")
        lang = "fa";
    else
        lang = "en";

    onlangchange();
}

function onlangchange()
{
    if (lang == "fa")
    {
        var toolbuttons = document.getElementsByClassName("tool");
        toolbuttons[0].innerHTML = "پاک کن";
        toolbuttons[1].innerHTML = "مداد";
        toolbuttons[2].innerHTML = "خالی کن";
        toolbuttons[3].innerHTML = "فارسی";
        document.getElementById("info").innerHTML = "این پروژه شبکه نورونی عمیق آموزش داده شده برای تشخیص اعداد فارسی دست نویس را به نمایش میگذارد<br/><br /> منبع این پروژه از این آدرس قابل درسترسی است:<br/><a style=\"color:rgb(154, 169, 239);font-size:15px;\" target=\"_blank\" href=\"https://github.com/manzik/Persian-Handwritten-Digit-Recognizer\">https://github.com/manzik/Persian-Handwritten-Digit-Recognizer</a>";
    }
    else
    {
        var toolbuttons = document.getElementsByClassName("tool");
        toolbuttons[0].innerHTML = "Eraser";
        toolbuttons[1].innerHTML = "Pen";
        toolbuttons[2].innerHTML = "Clear";
        toolbuttons[3].innerHTML = "English";
        document.getElementById("info").innerHTML = "This project demonstrates a deep neural network trained to detect persian numbers that have been trained using C++ for faster learning.<br/><br />Source code for this project is available at:<br/><a style=\"color:rgb(154, 169, 239);font-size:15px;\" target=\"_blank\" href=\"https://github.com/manzik/Persian-Handwritten-Digit-Recognizer\">https://github.com/manzik/Persian-Handwritten-Digit-Recognizer</a>";
    }
}

function cbuttonclick()
{
    for (var i = 0; i < 28 * 28; i++)
    {
        griddata[i] = 0;
    }

    rendergrid();

    //var p = mynet.predict(griddata.map(Math.round));

    for (var i = 0, leni = mynet.layers.length; i < leni; i++)
    {
        for (var j = 0, lenj = mynet.layers[i].neurons.length; j < lenj; j++)
        {
            mynet.layers[i].neurons[j].value = 0;
        }
    }
}

var buttonsstatepen = true;
function pbuttonclick()
{
    if (!buttonsstatepen)
    {
        buttonsstatepen = true;

        checkpstate();
    }
}
function ebuttonclick()
{
    if (buttonsstatepen)
    {
        buttonsstatepen = false;

        checkpstate();
    }
}

function checkpstate()
{
    var bs = document.getElementsByClassName("tool");
    if (buttonsstatepen)
    {
        bs[0].className = "tool";
        bs[1].className = "tool selected";
    }
    else
    {
        bs[1].className = "tool";
        bs[0].className = "tool selected";
    }
}
var c, cheight, cwidth;
function setsizes()
{
    try
    {
        c.width = innerWidth - 225;
        c.height = innerHeight;

        cheight = c.height;
        cwidth = c.width;
    }
    catch (e)
    {

    }

    neuronindexes = [];
    var ih = (innerHeight - 100);
    for (var i = 0; i < mynet.layers.length; i++)
    {
        var layer = mynet.layers[i];

        if (ih / (neuronsradius * 2 + neuronsmargin) < layer.neurons.length)
        {
            var newarr = [];
            for (var j = 0, lenj = (ih / (neuronsradius * 2 + neuronsmargin)); j < lenj; j++)
            {

                newarr.push(Math.round(j * (layer.neurons.length / lenj)));
            }
            neuronindexes.push(newarr);
        }
        else
        {
            var newarr = [];
            for (var j = 0, lenj = layer.neurons.length; j < lenj; j++)
            {
                newarr.push(j);
            }
            neuronindexes.push(newarr);
        }



    }
}
window.addEventListener("resize", function()
{
    setsizes();
});

window.addEventListener("load", function()
{
    var t = setInterval(function() { if (runifloaded()) { clearInterval(t) } }, 250);
});
function runifloaded()
{
    if (!netdata || !NeuralNetwork)
        return false;
    c = document.getElementById("c");
    ctx = c.getContext("2d");

    ic = document.getElementById("inputc");

    ictx = ic.getContext("2d");

    ic2 = document.getElementById("inputc2");
    ictx2 = ic.getContext("2d");

    ictx2.strokeStyle = "rgb(9,146,198)";

    for (var i = 0; i < 28 * 28; i++)
    {
        griddata[i] = 0;
    }

    rendergrid();

    numberscount = 10;

    mynet = new NeuralNetwork([28 * 28, 512, 512, numberscount]);

    mynet.loadNet(
        netdata
    );

    setsizes();
    render();

    drawdemo(Math.floor(Math.random() * 10));

    ic.addEventListener("touchstart", function (e)
    {
        var touch = e.touches[0];
        mouseinf.x = touch.clientX;
        mouseinf.y = touch.clientY;
        mouseinf.down = true;
    }, false);
    window.addEventListener("touchend", function (e)
    {
        var touch = e.touches[0];
        if (touch)
        {
            mouseinf.x = touch.clientX;
            mouseinf.y = touch.clientY;
        }
        mouseinf.down = false;

    }, false);
    ic.addEventListener("touchmove", function (e)
    {
        var touch = e.touches[0];
        mousemove(touch.clientX, touch.clientY);
        e.preventDefault();
    }, false);
    return true;
}
function drawdemo(number)
{
    cbuttonclick();

    ictx.drawImage(document.getElementById("number" + number), 0, 0, 250, 250);

    var scale = (250 / 28);
    for (var i = 0; i < 28 * 28; i++)
    {
        var x = i % 28 - 1;
        var y = Math.floor(i / 28) - 1;
        griddata[i] = ictx.getImageData(x * scale + 28 / 2, y * scale + 28 / 2, 1, 1).data[3] / 255;
    }

    rendergrid();

    var p = mynet.predict(griddata.map(Math.round));
}
var griddata = [];
function rendergrid()
{
    ictx2.lineWidth = 1;
    ictx2.strokeStyle = "rgb(9,146,198)";
    ictx2.clearRect(0, 0, 250, 250);
    for (var y = 0; y < 28; y++)
    {
        for (var x = 0; x < 28; x++)
        {
            var val = griddata[x + 28 * y];
            //console.log(val);
            var scale = (250 / 28);
            ictx2.fillStyle = "rgba(0,253,253," + val + ")";
            ictx2.beginPath();

            ictx2.moveTo(x * scale, y * scale);
            ictx2.lineTo(x * scale, (y + 1) * scale);
            ictx2.lineTo((x + 1) * scale, (y + 1) * scale);
            ictx2.lineTo((x + 1) * scale, y * scale);
            ictx2.lineTo(x * scale, y * scale);

            ictx2.fill();
        }
    }

    for (var x = 0; x < 250; x += 250 / 28)
    {
        ictx2.beginPath();
        ictx2.moveTo(Math.round(x) + 0.5, 0);
        ictx2.lineTo(Math.round(x) + 0.5, 250);
        ictx2.closePath();
        ictx2.stroke();
    }

    for (var y = 0; y < 250; y += 250 / 28)
    {
        ictx2.beginPath();
        ictx2.moveTo(0, Math.round(y) + 0.5);
        ictx2.lineTo(250, Math.round(y) + 0.5);
        ictx2.closePath();
        ictx2.stroke();
    }


}
var mouseinf = { x: 0, y: 0, down: false };

window.addEventListener("mousedown", function(e)
{
    mouseinf.x = e.offsetX;
    mouseinf.y = e.offsetY;
    mouseinf.down = true;
});
var mynet;
var neuronsmargin = 2;
var neuronsradius = 9;
neuronindexes = [];
var PI2 = Math.PI * 2;
var persiannumbers = "۰۱۲۳۴۵۶۷۸۹";
function renderneurons()
{
    var iw = innerWidth - 250;
    ctx.clearRect(0, 0, iw, innerHeight);

    var neurontotysize = (neuronsradius * 2 + neuronsmargin);
    ctx.strokeStyle = "rgba(9, 146, 198,0.05)";
    var ilni = (iw / (neuronindexes.length - 0.5));
    for (var i = 0, leni = neuronindexes.length; i < leni; i++)
    {
        for (var j = 0, lenj = neuronindexes[i].length; j < lenj; j++)
        {
            if(!this.z)
            {
                this.z=true;
                console.log(neuronindexes);
            }
            var neuronind = neuronindexes[i][j];
            var val=mynet.layers[i].neurons[neuronind].value;
            var ind=neuronind;
            /*
            for(var k=neuronind;k<((j+1<lenj)?neuronindexes[i][j+1]:neuronind+1);k++)
            {
                val+=mynet.layers[i].neurons[true?k:neuronind].value;
            }
            val/=(neuronindexes[i][j+1]-neuronind)||1;
            */
            var neuron = mynet.layers[i].neurons[neuronind];
            neuron.groupValue=val;
            neuron.groupAnimationValue -= (neuron.groupAnimationValue - neuron.groupValue) / 8;

            var val = neuron.groupAnimationValue;
            ctx.beginPath();
            ctx.arc(50 + i * ilni, (innerHeight) / 2 + neurontotysize * (j - lenj / 2), neuronsradius, 0, PI2);
            ctx.closePath();
            ctx.fillStyle = "rgba(9,146,198," + (val) + ")";
            ctx.fill();
            ctx.stroke();

            if (i == leni - 1)
            {
                ctx.font = ((lang == "en" ? 1 : 2) * neuronsradius * (1 + val / 2)) + "px Arial";
                ctx.fillText(lang == "en" ? String(j) : persiannumbers[j], 50 + i * ilni + neuronsradius * 2, neuronsradius / 2 + (innerHeight) / 2 + neurontotysize * (j - lenj / 2));
            }
        }
    }
    var ilni2 = (iw / (leni - 0.5));
    /*
    var neuroncountindratio=[];
    for(var i=0;i<neuronindexes.length;i++)
    {
        var rval=mynet.layers[i].neurons/neuronindexes[i].length;
        neuroncountindratio.push(rval);
    }
    */
    for (var i = 0, leni = neuronindexes.length; i < leni - 1; i++)
    {
        for (var j = 0, lenj = neuronindexes[i].length; j < lenj; j++)
        {

            var neuronind = neuronindexes[i][j];

            var val = mynet.layers[i].neurons[neuronind].groupAnimationValue;

            for (var j2 = 0, lenj2 = neuronindexes[i + 1].length; j2 < lenj2; j2++)
            {

                var weight = mynet.layers[i].neurons[neuronind].weights[neuronindexes[i + 1][j2]];


                ctx.beginPath();
                ctx.moveTo(50 + i * ilni2, (innerHeight) / 2 + neurontotysize * j - neurontotysize * lenj / 2);
                ctx.lineTo(50 + (i + 1) * ilni2, (innerHeight) / 2 + neurontotysize * (j2) - neurontotysize * neuronindexes[i + 1].length / 2)
                ctx.closePath();
                ctx.strokeStyle = "rgba(9, 146, 198," + (Math.max(0,weight) * val * mynet.layers[i + 1].neurons[neuronindexes[i + 1][j2]].groupAnimationValue) + ")";
                ctx.stroke();
            }
        }
    }
}
window.addEventListener("mouseup", function (e)
{
    mouseinf.x = e.offsetX;
    mouseinf.y = e.offsetY;
    mouseinf.down = false;

});
window.addEventListener("mousemove", function (e)
{
    mousemove(e.offsetX, e.offsetY);
});

function mousemove(newx, newy)
{
    if (mouseinf.down && mouseinf.x <= ic.width && mouseinf.y <= ic.height)
    {
        ictx.save();
        if (buttonsstatepen)
        {
            ictx.globalCompositeOperation = "";
        }
        else
        {
            ictx.globalCompositeOperation = "destination-out";
        }
        ictx.filter = "blur(1px)";
        ictx.lineWidth = 30;
        ictx.strokeStyle = "rgb(9,146,198)";
        ictx.lineJoin = "round";
        ictx.lineCap = "round";
        ictx.beginPath();
        ictx.moveTo(mouseinf.x, mouseinf.y);
        mouseinf.x = newx;
        mouseinf.y = newy;

        ictx.lineTo(mouseinf.x, mouseinf.y);
        ictx.closePath();
        ictx.stroke();
        ictx.restore();
        var scale = (250 / 28);
        for (var i = 0; i < 28 * 28; i++)
        {
            var x = i % 28 - 1;
            var y = Math.floor(i / 28) - 1;
            griddata[i] = ictx.getImageData(x * scale + 28 / 2, y * scale + 28 / 2, 1, 1).data[3] / 255;
        }

        rendergrid();

        var p = mynet.predict(griddata.map(Math.round));
        //renderneurons();
    }
}

function render()
{
    renderneurons()
    //ctx.fillStyle = "rgb(1,19,27)";
    //ctx.fillRect(0, 0, cwidth, cheight);
    requestAnimationFrame(render);
}
