inlets = 1;
outlets = 1;

function isInteger(x)
{
    x = parseFloat(x);
    return Math.round(x) === x;
}

function anything(a)
{
	var buffer = ArrayBuffer(4);
	
    var args = arrayfromargs(messagename, arguments);
    var outs = [];

    // address
    if (args[0][0] != "/") throw new Error("invalid address");
    for (var i = 0; i < args[0].length; ++i) outs.push(args[0].charCodeAt(i));

    // address alignment
    var remainder = args[0].length % 4;
    for (var i = 0; i < 4 - remainder; ++i)
        outs.push(0);

    // tags
    outs.push(",".charCodeAt(0));
    for (var i = 1; i < args.length; ++i)
    {
        if (isNaN(args[i]))
        {
            outs.push("s".charCodeAt(0));
        }
        else
        {
            if (isInteger(args[i])) outs.push("i".charCodeAt(0));
            else outs.push("f".charCodeAt(0));
        }
    }

    // tags alignment
    var remainder = outs.length % 4;
    for (var i = 0; i < 4 - remainder; ++i)
        outs.push(0);

    // arguments
    for (var i = 1; i < args.length; ++i)
    {
        if (isNaN(args[i]))
        {
            for (var j = 0; j < args[i].length; ++j)
                outs.push(args[i].charCodeAt(j));
            // string alignment
            var remainder = args[i].length % 4;
            for (var k = 0; k < 4 - remainder; ++k)
                outs.push(0);
        }
        else
        {
			var buffer = new ArrayBuffer(4);
			var arr;
            if (isInteger(args[i]))
			{
				arr = new Int32Array(buffer);
				arr[0] = args[i];
			}
            else 
			{
				arr = new Int32Array(buffer);
				var arr_ref = new Float32Array(buffer);
				arr_ref[0] = args[i];
			}
            outs.push((arr[0] & 0xFF000000) >> 24);
            outs.push((arr[0] & 0x00FF0000) >> 16);
            outs.push((arr[0] & 0x0000FF00) >>  8);
            outs.push((arr[0] & 0x000000FF) >>  0);
        }
    }


    outlet(0, outs);
}
