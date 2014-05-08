function t(msg, code) {
	console.print(msg);
	for (var i = 0; i < 10; i++) {
		console.print("\x1b[" + code + i + "m test ");
	}
	console.log("\x1b[0m");
}


t("\\ib[3?m   ", "3");
t("\\ib[4?m   ", "4");
t("\\ib[9?m   ", "9");
t("\\ib[10?m  ", "10");
t("\\ib[0;3?m ", "0;3");
t("\\ib[0;4?m ", "0;4");
t("\\ib[1;3?m ", "1;3");
t("\\ib[1;4?m ", "1;4");
