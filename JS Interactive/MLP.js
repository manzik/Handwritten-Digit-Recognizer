
class NeuralNetwork
{

    constructor(sizes)
    {
        this.learningrate = 0.3;
        this.sizes = sizes;
        this.layers = [];
        for (var i = 0; i < sizes.length; i++)
        {
            this.layers.push(new Layer(sizes[i], i, this));
        }
        this.errors = { i: 0, value: 0, percent: 0 };
    }
    loadNet(arr_weights)
    {
        var iii = 0;

        this.layers.forEach((layer) =>
        {
            layer.neurons.forEach((neuron) =>
            {
                for (var i = 0; i < neuron.weights.length; i++)
                {
                    neuron.weights[i] = arr_weights[iii++];
                }
            });
        });
    }
    startAutoRendering()
    {
        setInterval(() => { this.render(); }, 100);
    }
    render()
    {
        var ctx = this.ctx;

        var netlist = this.getAsList();
        var xspace = 70;
        var yspace = 70;
        var maxlen = netlist.map((x) => { return x.length }).reduce((a, b) => { return Math.max(a, b) }, 1);

        ctx.clearRect(0, 0, innerWidth, innerHeight)
        netlist.forEach((layer, i) =>
        {
            layer.forEach((neuron, j) =>
            {
                neuron.cx = (i + 0.5) * xspace;
                neuron.cy = (j + (maxlen - layer.length) / 2 + 0.5) * yspace;
            });
        });
        netlist.forEach((layer, i) =>
        {
            layer.forEach((neuron, j) =>
            {

                neuron.weights.forEach((weight, j2) =>
                {
                    ctx.beginPath();
                    ctx.moveTo(neuron.cx, neuron.cy);
                    var tarneuron = netlist[i + 1][j2];
                    ctx.lineTo(tarneuron.cx, tarneuron.cy);
                    ctx.closePath();
                    ctx.lineWidth = Math.abs(weight);
                    ctx.strokeStyle = weight > 0 ? "rgb(0,255,0)" : "rgb(255,0,0)"
                    ctx.stroke();

                    ctx.font = "8px Arial";


                    ctx.fillStyle = "blue"; ctx.fillText(String(weight.toFixed(3)), (neuron.cx * 2 + tarneuron.cx) / 3 - 10, (neuron.cy * 2 + tarneuron.cy) / 3 + 2);
                });

            });

            ctx.fillStyle = "black";

            ctx.font = "16px Arial";

            ctx.fillText(String(this.errors.percent), 20, 20);
        });
        netlist.forEach((layer, i) =>
        {
            layer.forEach((neuron, j) =>
            {
                ctx.beginPath();
                var cx = neuron.cx;//(i+0.5)*xspace;
                var cy = neuron.cy;//(j+(maxlen-layer.length)/2+0.5)*yspace;
                ctx.arc(cx, cy, 15, 0, Math.PI * 2);
                ctx.closePath();
                ctx.fillStyle = i > 0 ? "rgb(200,200,200)" : "rgb(180,180,255)";
                ctx.fill();

                ctx.font = "8px Arial";


                if (i == netlist.length - 1)
                {
                    ctx.fillStyle = "green";
                    ctx.fillText(String(this.wantedoutputs[j].toFixed(3)), cx - 10 + 30, cy + 2);

                }
                ctx.fillStyle = "black";
                ctx.fillText(String(neuron.value.toFixed(3)), cx - 10, cy + 2);
            })
        });
    }
    setCanvas(c)
    {
        this.c = c;
        this.ctx = c.getContext("2d");
    }

    predict(inputs)
    {
        // console.log(this.sizes[0])
        if (inputs.length != this.sizes[0])
        {
            console.log(inputs.length, this.sizes[0])
            throw new Error("input sizes do not match");
        }

        for (var i = 0; i < inputs.length; i++)
        {
            this.layers[0].neurons[i].value = inputs[i];
        }

        this.layers.forEach((layer, i) =>
        {
            if (i > 0)
                layer.neurons.forEach((neuron, j) =>
                {
                    neuron.value = neuron.getPrevLayer().neurons.map((prevneuron) => { return prevneuron.value * prevneuron.getNextWeight(j) }).reduce((a, b) => { return a + b }, 0);
                    neuron.V = neuron.value;
                    neuron.value = this.sigmoid(neuron.value);
                });
        });
        var netlist = this.getAsList();
        var outputs = netlist[netlist.length - 1].map((x) => { return x.value });
        return outputs;
    }

    learn(wantedoutputs)
    {
        // console.log(wantedoutputs.matrix)
        if (wantedoutputs.length != this.sizes[this.sizes.length - 1])
            throw new Error("output sizes do not match");

        this.wantedoutputs = wantedoutputs;
        var netlist = this.getAsList();
        var outputs = netlist[netlist.length - 1].map((x) => { return x.value });

        var error = 0;
        //console.log(JSON.stringify(wantedoutputs));
        error = outputs.map((out, i) => { return (Math.pow(wantedoutputs[i] - out, 2) - 0.5) * 2 }).reduce((a, b) => { return (a + b) / 2 }, 1);
        //console.log(error)

        if (this.errors.i > 500)
        {
            this.errors.value = 0;
            this.errors.i = 0;
        }
        this.errors.value += error;

        this.errors.percent = this.errors.value / (++this.errors.i);



        this.layers[this.layers.length - 1].neurons.forEach((neuron, j) =>
        {
            neuron.J = this.sigmoid_derivative(neuron.V) * (wantedoutputs[j] - neuron.value);
        });

        for (var i = this.layers.length - 2; i >= 0; i--)
        {
            var layer = this.layers[i];

            layer.neurons.forEach((neuron, j) =>
            {
                var avg_wj = 0;
                layer.getNextLayer().neurons.forEach((neuron2, j2) =>
                {
                    neuron.addWeights[j2] = neuron.value * neuron2.J * this.learningrate;
                    avg_wj += neuron2.J * neuron.weights[j2];
                });
                avg_wj /= layer.getNextLayer().neurons.length;
                if (i > 0)
                    neuron.J = this.sigmoid_derivative(neuron.V) * avg_wj;
                else
                    neuron.J = avg_wj;
            });

        }

        this.layers.forEach((layer, i) =>
        {
            layer.neurons.forEach((neuron, j) =>
            {
                neuron.applyWeightChanges();
            });
        });

        return this.layers[0].neurons.map((x) => { return x.J })

        //   console.log(JSON.stringify(this.errors))
    }
    // resetAll()
    sigmoid(x)
    {
        return 1 / (1 + Math.pow(Math.E, -x));
    }

    sigmoid_derivative(x)
    {
        return this.sigmoid(x) * (1 - this.sigmoid(x));
    }



    sigmoid_inverse(x)
    {
        return Math.log(1 / x - 1);
    }


    getAsList()
    {
        return this.layers.map((x) => { return x.neurons.map((y) => { return { value: y.value, weights: y.weights } }) });
    }
    textNetwork()
    {
        var network = this.getAsList();
        var maxlen = network.map((x) => { return x.length }).reduce((a, b) => { return Math.max(a, b) }, 1);
        var totstr = "";
        for (var i = 0; i < maxlen; i++)
        {
            var str = "";
            for (var j = 0; j < network.length; j++)
            {
                if (str.length < j * 10)
                    str += " ".repeat(j * 10 - str.length);
                if (network[j][i])
                    str += network[j][i].value.toFixed(5);
            }
            totstr += str + "\n";
        }
        return totstr;
    }

}
class Layer
{
    getNextLayer()
    {
        return this.parent.layers[this.index + 1];
    }
    getPrevLayer()
    {
        return this.parent.layers[this.index - 1];
    }
    constructor(size, index, parent)
    {
        this.neurons = [];

        this.index = index;
        this.size = size;
        this.parent = parent;
        for (var i = 0; i < size; i++)
        {
            this.neurons.push(new Neuron(i, this));
        }
    }
}
class Neuron
{
    constructor(index, parent)
    {
        this.weights = [];
        this.addWeights = [];
        for (var i = 0; i < parent.parent.sizes[parent.index + 1]; i++)
        {
            this.weights[i] = Math.random() * 2 - 1;
            this.addWeights[i] = 0;
        }
        this.index = index;
        this.parent = parent;
        this.value = 0;
        this.animationValue = 0;
        this.V = 0;
        this.K = 0
    }

    applyWeightChanges()
    {
        for (var i = 0; i < this.addWeights.length; i++)
        {
            this.weights[i] += this.addWeights[i];
            this.addWeights[i] = 0;
        }
    }


    getNextLayer()
    {
        return this.parent.getNextLayer();
    }
    getPrevLayer()
    {
        return this.parent.getPrevLayer();
    }

    getNextNeuron(j)
    {
        return this.parent.getNextLayer()[j];
    }

    getPrevNeuron(j)
    {
        return this.parent.getPrevLayer()[j];
    }


    getNextWeight(j)
    {
        return this.weights[j];
    }

    getPrevWeight(j, val)
    {
        this.weights[j] = val;
    }

    getLayer()
    {
        return this.parent;
    }
}