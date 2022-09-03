let __lines = require("fs").readFileSync(0).toString().split("\n");
let input = () => __lines.shift();
let write = text => process.stdout.write("" + text);
let print = text => console.log(text);

function main() {
    let chain = new Map();
    let ui = [];

    chain.set("soma", () => {
        print(+ui[1] + +ui[2]);
    });

    chain.set("double", () => {
        print(+ui[1] * +ui[1])
    });

    chain.set("sub", () => {
        print(+ui[1] - +ui[2])
    });

    shell(chain, ui, false);
}

function shell(chain, ui) {
    while (true) {
        let line = input();
        print("$" + line);

        ui.splice(0); //apagar tudo
        line.split(" ").forEach(x => ui.push(x));
        
        let cmd = ui[0];
        if (cmd == "end") {
            return;
        } else if (chain.has(cmd)) {
            chain.get(cmd)();
        } else {
            print("fail: command not found");
        }
    }
}

main()


