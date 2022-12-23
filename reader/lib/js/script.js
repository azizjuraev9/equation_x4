$(function(){
	let sData = [];
	for(let i = 0; i <= 100; i++){
		sData.push( (i/100).toFixed(6) );
	}
	
	$("select").select2({
		data: sData,
		theme: "bootstrap-5",
		width: 150
	});
	
	config = {
      locateFile: filename => `${filename}`
    }
	
    // The `initSqlJs` function is globally provided by all of the main dist files if loaded in the browser.
    // We must specify this locateFile function if we are loading a wasm file from anywhere other than the current html page's folder.
    initSqlJs(config).then(async function(SQL){
      //Create the database
		const fetched = await fetch("results.db");
		const buf = await fetched.arrayBuffer();
		const db = new SQL.Database(new Uint8Array(buf))
				
		$("#search").click(function(e){
			e.defaultPrevented = true;
			e.preventDefault();
			
			let x1 = $("#x1").val();
			let x2 = $("#x2").val();
			let x3 = $("#x3").val();
			let x4 = $("#x4").val();
			let group_name = x1 + "-" + x2 + "-" + x3 + "-" + x4;
			
			const stmt = db.prepare("SELECT x1, x2, x3, x4 FROM results WHERE group_name=\""+ group_name +"\"");
			stmt.getAsObject();
			
			stmt.bind({$x1:1, $x2:2, $x3:3, $x4:4});
			$("tbody").html("");
			while(stmt.step()) { //
				const row = stmt.getAsObject();
				console.log(row);
				$("tbody").append("<tr><td>"+row.x1+"</td><td>"+row.x2+"</td><td>"+row.x3+"</td><td>"+row.x4+"</td></tr>");
			}
			
		});
    });
});