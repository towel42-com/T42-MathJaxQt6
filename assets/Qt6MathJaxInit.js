MathJax = {
    svg: { fontCache: 'none' },
    tex: {
        packages: { '[+]': ['texhtml'] },
        formatError(jax, error) {
            msg = `TeX error in "${jax.latex}": ${error.message}`;
            console.log(msg);
            if (qt6MathJax != null) {
                qt6MathJax.errorMessage(msg);
            }
            return jax.formatError(error);
        }
    },
    options: {
        enableMenu: true,
        enableSpeech: false,       // false to disable speech strings
        enableBraille: false,      // false to disable Braille notation
        a11y: {
            speech: false,           // switch on speech output when enabled
            braille: false,          // switch on Braille output when enabled
        }
    },
    startup: {
        ready() {
            MathJax.startup.defaultReady();
            const doc = MathJax.startup.document;
        }
    }
};
