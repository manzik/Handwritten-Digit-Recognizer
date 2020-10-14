let networkLang = "en";
let currentNetwork, networkWeights;
let networkLangsData = {
    fa: { filePath: "netdata-persian.json.lzstring", data: null },
    en: { filePath: "netdata-english-transformed.json.lzstring", data: null }
};

let faLangNeuralNet = null, engLangNeuralNet;

function networkLangChange(newNetworkLang)
{
    if(networkLang != newNetworkLang)
    {
        if (networkLang == "en")
            networkLang = "fa";
        else
            networkLang = "en";
        
        setCurrentNetwork(newNetworkLang, () => 
        {
            applyNetworkLangChangeToUI(newNetworkLang)
        });
    }
}

function applyNetworkLangChangeToUI(newLang)
{
    let isNewEn = newLang == "en";

    let languageElementsArr = document.querySelector("#langs-toolbar").children;
    Array.from(languageElementsArr).forEach((languageElement, ind) => 
    {
        let isActivated = isNewEn ? ind == 0 : ind == 1;
        if(isActivated)
            languageElement.classList.add("selected");
        else
            languageElement.classList.remove("selected"); 
    });
}

let neuronsInfo = { x: 0, y: 0, down: false };
let neuronsMargin = 2;
let neuronsRadius = 9;
let neuronIndexes = [];
let PI2 = Math.PI * 2;
let persianNumbers = "۰۱۲۳۴۵۶۷۸۹";

function setCurrentNetwork(newLang, cb)
{
    let networkLangData = networkLangsData[newLang];

    let setNetworkFromWeights = () => 
    { 
        networkWeights = networkLangData.data;
        if(currentNetwork)
        {
            currentNetwork.loadNet(networkWeights);
            currentNetwork.predict(gridData.map(Math.round));
        }

        if(cb)
            cb();
    };

    // Either load neural network weights to networkLangData.data and set neural network data or if it's there do it right away
    if(!networkLangData.data)
    {
        toastr.info(`Downloading pretrained parameters for the ${newLang == "fa" ? "Persian" : "English"} language`, {timeOut: 5000});
        fetch(networkLangData.filePath)
        .then(response => response.text())
        .then(response => 
        {
            let decompressedNetworkWeights = LZString.decompressFromBase64(response);
            networkLangData.data = JSON.parse(decompressedNetworkWeights);
            setNetworkFromWeights();
            toastr.info(`Downloaded, decoded and set parameters for the ${newLang == "fa" ? "Persian" : "English"} language`, {timeOut: 5000});
            decompressedNetworkWeights = null;
            response = null;
        })
        .catch((e) => {
            toastr.info(`Error occured while downloading paramters for the ${newLang == "fa" ? "Persian" : "English"} language`, {timeOut: 5000});
        });
    }
    else
        setNetworkFromWeights();
}

function clearButtonClick()
{
    for (let i = 0; i < 28 * 28; i++)
    {
        gridData[i] = 0;
    }

    renderGrid();

    for (let i = 0, leni = currentNetwork.layers.length; i < leni; i++)
    {
        for (let j = 0, lenj = currentNetwork.layers[i].neurons.length; j < lenj; j++)
        {
            currentNetwork.layers[i].neurons[j].value = 0;
        }
    }
}

let buttonStateIsPen = true;
function penButtonClick()
{
    if (!buttonStateIsPen)
    {
        buttonStateIsPen = true;

        setActiveCanvasButton();
    }
}
function eraseButtonClick()
{
    if (buttonStateIsPen)
    {
        buttonStateIsPen = false;

        setActiveCanvasButton();
    }
}

function setActiveCanvasButton()
{
    let canvasToolButtonElementsArr = document.querySelectorAll(".canvas-toolbar > button");
    Array.from(canvasToolButtonElementsArr).slice(0, 2).forEach(languageElement => 
    { 
        languageElement.classList.toggle("selected"); 
    });
}
let networkCanvas, canvasHeight, canvasWidth;
function setSizes()
{
    try
    {
        networkCanvas.width = innerWidth - 225;
        networkCanvas.height = innerHeight;

        canvasHeight = networkCanvas.height;
        canvasWidth = networkCanvas.width;
    }
    catch (e) { }

    neuronIndexes = [];
    let ih = (innerHeight - 100);
    for (let i = 0; i < currentNetwork.layers.length; i++)
    {
        let layer = currentNetwork.layers[i];

        if (ih / (neuronsRadius * 2 + neuronsMargin) < layer.neurons.length)
        {
            let newarr = [];
            for (let j = 0, lenj = (ih / (neuronsRadius * 2 + neuronsMargin)); j < lenj; j++)
            {

                newarr.push(Math.round(j * (layer.neurons.length / lenj)));
            }
            neuronIndexes.push(newarr);
        }
        else
        {
            let newarr = [];
            for (let j = 0, lenj = layer.neurons.length; j < lenj; j++)
            {
                newarr.push(j);
            }
            neuronIndexes.push(newarr);
        }
    }
}
window.addEventListener("resize", function ()
{
    setSizes();
});
window.addEventListener("load", function ()
{
    setCurrentNetwork(networkLang);
    let t = setInterval(function () { if (runRenderingLoopIfLoaded()) { clearInterval(t) } }, 250);
});
function runRenderingLoopIfLoaded()
{
    if (!networkWeights || !NeuralNetwork)
        return false;
        
    networkCanvas = document.getElementById("network-canvas");
    networkCtx = networkCanvas.getContext("2d");

    visibleDrawingCanvas = document.getElementById("visible-draw-canvas");

    visibleDrawingCtx = visibleDrawingCanvas.getContext("2d");

    hiddenDrawingCanvas = document.getElementById("hidden-draw-canvas");
    hiddenDrawingCtx = visibleDrawingCanvas.getContext("2d");

    hiddenDrawingCtx.strokeStyle = "rgb(9,146,198)";

    for (let i = 0; i < 28 * 28; i++)
    {
        gridData[i] = 0;
    }

    renderGrid();

    let digitsCount = 10;

    currentNetwork = new NeuralNetwork([28 * 28, 512, 512, digitsCount]);

    currentNetwork.loadNet(networkWeights);

    setSizes();
    render();

    drawDemo(Math.floor(Math.random() * 10));

    visibleDrawingCanvas.addEventListener("touchstart", function (e)
    {
        let touch = e.touches[0];
        neuronsInfo.x = touch.clientX;
        neuronsInfo.y = touch.clientY;
        neuronsInfo.down = true;
    }, false);
    window.addEventListener("touchend", function (e)
    {
        let touch = e.touches[0];
        if (touch)
        {
            neuronsInfo.x = touch.clientX;
            neuronsInfo.y = touch.clientY;
        }
        neuronsInfo.down = false;

    }, false);
    visibleDrawingCanvas.addEventListener("touchmove", function (e)
    {
        let touch = e.touches[0];
        mouseMove(touch.clientX, touch.clientY);
        e.preventDefault();
    }, false);
    return true;
}
function drawDemo(number)
{
    clearButtonClick();

    hiddenDrawingCtx.drawImage(document.getElementById(`${networkLang == "en" ? "english" : "persian"}-number-${number}`), 0, 0, 250, 250);

    let scale = (250 / 28);
    for (let i = 0; i < 28 * 28; i++)
    {
        let x = i % 28 - 1;
        let y = Math.floor(i / 28) - 1;
        gridData[i] = hiddenDrawingCtx.getImageData(x * scale + 28 / 2, y * scale + 28 / 2, 1, 1).data[3] / 255;
    }

    renderGrid();

    let p = currentNetwork.predict(gridData.map(Math.round));
}
let gridData = [];
function renderGrid()
{
    hiddenDrawingCtx.lineWidth = 1;
    hiddenDrawingCtx.strokeStyle = "rgb(9,146,198)";
    hiddenDrawingCtx.clearRect(0, 0, 250, 250);
    for (let y = 0; y < 28; y++)
    {
        for (let x = 0; x < 28; x++)
        {
            let val = gridData[x + 28 * y];
            let scale = (250 / 28);

            hiddenDrawingCtx.fillStyle = "rgba(0,253,253," + val + ")";
            hiddenDrawingCtx.beginPath();

            hiddenDrawingCtx.moveTo(x * scale, y * scale);
            hiddenDrawingCtx.lineTo(x * scale, (y + 1) * scale);
            hiddenDrawingCtx.lineTo((x + 1) * scale, (y + 1) * scale);
            hiddenDrawingCtx.lineTo((x + 1) * scale, y * scale);
            hiddenDrawingCtx.lineTo(x * scale, y * scale);

            hiddenDrawingCtx.fill();
        }
    }

    for (let x = 0; x < 250; x += 250 / 28)
    {
        hiddenDrawingCtx.beginPath();
        hiddenDrawingCtx.moveTo(Math.round(x) + 0.5, 0);
        hiddenDrawingCtx.lineTo(Math.round(x) + 0.5, 250);
        hiddenDrawingCtx.closePath();
        hiddenDrawingCtx.stroke();
    }

    for (let y = 0; y < 250; y += 250 / 28)
    {
        hiddenDrawingCtx.beginPath();
        hiddenDrawingCtx.moveTo(0, Math.round(y) + 0.5);
        hiddenDrawingCtx.lineTo(250, Math.round(y) + 0.5);
        hiddenDrawingCtx.closePath();
        hiddenDrawingCtx.stroke();
    }


}

function isEventTargetDrawingCanvas(e)
{
    return e.target.id == "visible-draw-canvas";
}

window.addEventListener("mousedown", function (e)
{
    if(!isEventTargetDrawingCanvas(e))
        return;
    
    neuronsInfo.x = e.offsetX;
    neuronsInfo.y = e.offsetY;
    neuronsInfo.down = true;
});
window.addEventListener("mouseup", function (e)
{
    neuronsInfo.x = e.offsetX;
    neuronsInfo.y = e.offsetY;
    neuronsInfo.down = false;
});
window.addEventListener("mousemove", function (e)
{
    if(!isEventTargetDrawingCanvas(e))
        return;

    mouseMove(e.offsetX, e.offsetY);
});

function renderNeurons()
{
    let iw = innerWidth - 250;
    networkCtx.clearRect(0, 0, iw, innerHeight);

    let neurontotysize = (neuronsRadius * 2 + neuronsMargin);
    networkCtx.strokeStyle = "rgba(9, 146, 198,0.05)";
    let renderingNeuronsLayersCount = neuronIndexes.length;
    let ilni = (iw / (renderingNeuronsLayersCount - 0.5));
    for (let i = 0, leni = renderingNeuronsLayersCount; i < leni; i++)
    {
        for (let j = 0, lenj = neuronIndexes[i].length; j < lenj; j++)
        {
            let neuronInd = neuronIndexes[i][j];

            if(currentNetwork.layers[i].neurons.length <= neuronInd)
                continue;

            let val = currentNetwork.layers[i].neurons[neuronInd].value;
            let ind = neuronInd;
            
            let neuron = currentNetwork.layers[i].neurons[neuronInd];
            neuron.groupValue = val;
            neuron.groupAnimationValue -= (neuron.groupAnimationValue - neuron.groupValue) / 8;

            let neuronActivationStrength = neuron.groupAnimationValue;
            networkCtx.beginPath();
            networkCtx.arc(50 + i * ilni, (innerHeight) / 2 + neurontotysize * (j - lenj / 2), neuronsRadius, 0, PI2);
            networkCtx.closePath();
            networkCtx.fillStyle = "rgba(9,146,198," + (neuronActivationStrength) + ")";
            networkCtx.fill();
            networkCtx.stroke();

            if (i == leni - 1)
            {
                networkCtx.font = (neuronsRadius * (1 + val / 2)) + "px Arial";
                networkCtx.fillText(networkLang == "en" ? String(j) : `${String(j)} (${persianNumbers[j]})`, 50 + i * ilni + neuronsRadius * 2, neuronsRadius / 2 + (innerHeight) / 2 + neurontotysize * (j - lenj / 2));
            }
        }
    }
    let ilni2 = (iw / (renderingNeuronsLayersCount - 0.5));
    
    for (let i = 0, leni = neuronIndexes.length; i < leni - 1; i++)
    {
        for (let j = 0, lenj = neuronIndexes[i].length; j < lenj; j++)
        {

            let neuronInd = neuronIndexes[i][j];

            let targetNeuron = currentNetwork.layers[i].neurons[neuronInd];

            if(!targetNeuron)
                continue;

            let val = targetNeuron.groupAnimationValue;

            for (let k = 0, lenj2 = neuronIndexes[i + 1].length; k < lenj2; k++)
            {

                let weight = currentNetwork.layers[i].neurons[neuronInd].weights[neuronIndexes[i + 1][k]];

                if(!weight)
                    continue;

                networkCtx.beginPath();
                networkCtx.moveTo(50 + i * ilni2, (innerHeight) / 2 + neurontotysize * j - neurontotysize * lenj / 2);
                networkCtx.lineTo(50 + (i + 1) * ilni2, (innerHeight) / 2 + neurontotysize * (k) - neurontotysize * neuronIndexes[i + 1].length / 2)
                networkCtx.closePath();
                networkCtx.strokeStyle = "rgba(9, 146, 198," + (Math.max(0, weight) * val * currentNetwork.layers[i + 1].neurons[neuronIndexes[i + 1][k]].groupAnimationValue) + ")";
                networkCtx.stroke();
            }
        }
    }
}

function mouseMove(newx, newy)
{
    if (neuronsInfo.down && neuronsInfo.x <= visibleDrawingCanvas.width && neuronsInfo.y <= visibleDrawingCanvas.height)
    {
        hiddenDrawingCtx.save();
        if (buttonStateIsPen)
        {
            hiddenDrawingCtx.globalCompositeOperation = "";
        }
        else
        {
            hiddenDrawingCtx.globalCompositeOperation = "destination-out";
        }
        hiddenDrawingCtx.filter = "blur(3px)";
        hiddenDrawingCtx.lineWidth = 25;
        hiddenDrawingCtx.strokeStyle = "rgb(0,0,0)";
        hiddenDrawingCtx.lineJoin = "round";
        hiddenDrawingCtx.lineCap = "round";
        hiddenDrawingCtx.beginPath();
        hiddenDrawingCtx.moveTo(neuronsInfo.x, neuronsInfo.y);
        neuronsInfo.x = newx;
        neuronsInfo.y = newy;

        hiddenDrawingCtx.lineTo(neuronsInfo.x, neuronsInfo.y);
        hiddenDrawingCtx.closePath();
        hiddenDrawingCtx.stroke();
        hiddenDrawingCtx.restore();
        let scale = (250 / 28);
        for (let i = 0; i < 28 * 28; i++)
        {
            let x = i % 28 - 1;
            let y = Math.floor(i / 28) - 1;
            gridData[i] = hiddenDrawingCtx.getImageData(x * scale + 28 / 2, y * scale + 28 / 2, 1, 1).data[3] / 255;
        }

        renderGrid();

        currentNetwork.predict(gridData.map(Math.round));
    }
}

function render()
{
    renderNeurons();
    requestAnimationFrame(render);
}
