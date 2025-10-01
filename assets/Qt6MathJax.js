var qt6MathJax = null;
var outputDiv = document.querySelector('#MathOutput');
if (outputDiv == null) {
    console.warn("Could not find output <div>");
} else {
    console.log("Found output <div>");
}
function reportException(e) {
    if (outputDiv != null) {
        outputDiv.innerHTML = `<pre>Error: ${e.message}</pre>`;
    }

    var message = "";
    if (e.fileName != null) {
        message += e.fileName;
    }
    if ((e.lineNumber != null) || (e.columnNumber != null)) {
        message += "(";
        if (e.lineNumber != null) {
            message += e.lineNumber;
        }

        if ((e.lineNumber != null) && (e.columnNumber != null)) {
            message += ",";
        }

        if (e.columnNumber != null) {
            message += e.columnNumber;
        }
        message += ")";
    }
    if (message.length != 0) {
        message += ": ";
    }
    message += e.message;

    console.error(message);

    if (qt6MathJax != null) {
        qt6MathJax.renderingFinished();
    }
};

document.addEventListener("DOMContentLoaded", function () {
    console.log("DOM Content Loaded");
    try {
        new QWebChannel(qt.webChannelTransport,
            function (channel) {
                qt6MathJax = channel.objects.qt6MathJax;
                console.log("Channel Configured");
            }
        );
    } catch (e) {
        reportException(e);
    }
});

var Renderer = function () {
    this.render = async function (input) {
        try {
            if (qt6MathJax == null) {
                throw new Error("Qt Interface was not properly initialized");
            }

            if (MathJax == null) {
                throw new Error("MathJax was not propery initialized");
            }

            if (input == null) {
                input = document.querySelector('#input').value.trim();

                if ((input == null) || (input.length == 0)) {
                    throw new Error("Could not find the input, the input was not sent in or the input was empty.");
                }
            }

            console.log("Input: '" + input + "'");

            var options = null;
            if (outputDiv != null) {
                options = MathJax.getMetricsFor(outputDiv);
            }

            MathJax.texReset();
            MathJax.startup.document.clear();

            console.log("Calling tex2svg");
            await MathJax.tex2svgPromise(input, options).then((node) => {
                console.log("MathJax has finished rendering the SVG");
                if (outputDiv != null) {
                    console.log("Updating output");
                    outputDiv.innerHTML = '';
                    outputDiv.appendChild(node);
                    MathJax.startup.document.updateDocument();
                } else {
                    console.log("Output not found");
                }

                for (const child of node.children) {
                    if (child.tagName == "svg") {
                        qt6MathJax.svgRendered(child.outerHTML);
                        qt6MathJax.renderingFinished();
                    }
                }
            });
        } catch (e) {
            reportException(e);
        }
        console.log("Post Async Call");
        return "Started";
    }
}
