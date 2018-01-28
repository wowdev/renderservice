    var renderServiceURL = "http://render.marlamin.com:2082/";
    var endPoint = "wow/m2";

    var genericArgs = {
        "backgroundColor" : {
            "type" : "color",
            "default" : "0"   // hexcolor = 0
        },
        "portalsEnabled" : {
            "type" : "bool",
            "default" : "true" // bool = true
        },
        "fogEnabled" : {
            "type" : "bool",
            "default" : "true" // bool = true
        },
        "lightingEnabled" : {
            "type" : "bool",
            "default" : "true" // bool = true
        },
        "ffxEnabled" : {
            "type" : "bool",
            "default" : "true" // bool = true
        },
        "showDetailDoodads" : {
            "type" : "bool",
            "default" : "true" // bool = true
        },
        "showDoodads" : {
            "type" : "bool",
            "default" : "true" // bool = true
        },
        "showLiquid" : {
            "type" : "bool",
            "default" : "true" // bool = true
        },
        "showMapObjs" : {
            "type" : "bool",
            "default" : "true" // bool = true
        },
        "showParticles" : {
            "type" : "bool",
            "default" : "true" // bool = true
        },
        "showSky" : {
            "type" : "bool",
            "default" : "false" // bool = false
        },
        "showTerrain" : {
            "type" : "bool",
            "default" : "true"  // bool = true
        },
        "showOccluders" : {
            "type" : "bool",
            "default" : "false" // bool = false
        },
        "showPortals" : {
            "type" : "bool",
            "default" : "false" // bool = false
        },
        "showGrid" : {
            "type" : "bool",
            "default" : "false" // bool = false
        },
        "overDraw" : {
            "type" : "int",
            "default" : "0" // int = 0
        },
        "imgSize" : {
            "type" : "text" // int,int
        },
        "animSequence" : {
            "type" : "int" // int
        },
        "animVariation" : {
            "type" : "int" // int
        },
        "animTimer" : {
            "type" : "int", // int
        },
        "animRot" : {
            "type" : "text", // float
        },
        "animShowWeapons" : { // bool
            "type" : "bool",
            "default" : "false"
        },
        "iaCrossFade" : {
            "type" : "int"
        },
        "iaFramesPerSecond" : {
            "type" : "int"
        },
        "irFrameCount" : {
            "type" : "int"
        },
        "irRotateDeltaRadians" : {
            "type" : "text", // float
        },
        "nearClip" : {
            "type" : "text", // float
        },
        "farClip" : {
            "type" : "text", // float
        },
        "fov" : {
            "type" : "text", // float
        },
        "ortho" : { // bool
            "type" : "bool",
            "default" : "false"
        },
        "scale" : {
            "type" : "text", // float
        },
        "yaw" : {
            "type" : "text", // float
        },
        "pitch" : {
            "type" : "text", // float
        },
        "fitToScreen" : { // bool
            "type" : "bool",
            "default" : "false"
        },
        "trans" : {
            "type" : "text", // float, float, float
        },
    };

    var m2Options = {
        "enableGeosetList" : {
            "type" : "bool"
        },
        "geoset" : {
            "type" : "text"
        }
    };

    var wmoOptions = {
        "miniMap" : {
            "type" : "bool"
        },
        "chunkIndex" : {
            "type" : "int"
        },
        // map also has these and that causes issues :( todo fix
        // "lightDir" : {
        //     "type" : "text"
        // },
        // "lightCol" : {
        //     "type" : "text"
        // },
        // "ambCol" : {
        //     "type" : "text"
        // },
        "snapShot" : {
            "type" : "bool"
        }
    };

    var mapOptions = {
        "pos" : {
            "type" : "text"
        },
        "coord" : {
            "type" : "text"
        },
        "rect" : {
            "type" : "text"
        },
        "lightDir" : {
            "type" : "text"
        },
        "lightCol" : {
            "type" : "text"
        },
        "ambCol" : {
            "type" : "text"
        },
        "areaLightWater" : {
            "type" : "bool"
        },
    };

    // Generate inputs
    generateInputs("generic", genericArgs);
    generateInputs("m2", m2Options);
    generateInputs("wmo", wmoOptions);
    generateInputs("maps", mapOptions);

    // Refresh endPoint options once
    toggleEndPointOptions();

    // Update endPoint specific inputs when radio selection changes
    var endpoints = document.querySelectorAll('input[name="endpoint"]');
    Array.prototype.forEach.call(endpoints, function(radio) {
        radio.addEventListener('change', endpointChange);
    });

    // Sidebar button
    document.getElementById( 'js-sidebar-button' ).addEventListener( 'click', function( )
    {
        document.getElementById( 'js-sidebar' ).classList.toggle( 'closed' );
    } );

    // Generics button
    document.getElementById( 'js-generics-button' ).addEventListener( 'click', function( )
    {
        document.getElementById( 'js-generics' ).classList.toggle( 'closed' );
    } );

    // Don't "submit" the form but call sendRequest() instead
    document.querySelector("#optionForm").addEventListener("submit", function(e){
        e.preventDefault();
        sendRequest();
    });
    function endpointChange(event) {
        console.log("Endpoint change: " + this.value);
        endPoint = this.value;
        toggleEndPointOptions(endPoint);
        var label = document.getElementById("arg1label");
        switch(this.value){
            case "wow/maps":
            label.innerHTML = 'mapid';
            break;
            case "wow/wmo":
            case "wow/m2":
            label.innerHTML = 'filedataid';
            break;
            default:
            label.innerHTML = 'arg 1';
            break;
        }
    }

    function toggleEndPointOptions(){
        var opts = document.querySelectorAll('.opt');
        Array.prototype.forEach.call(opts, function(opt) {
            if(opt.dataset.endpoint != "wow/generic"){
                if(endPoint != opt.dataset.endpoint){
                    opt.style.display = 'none';
                    document.getElementById(opt.name + "Label").style.display = 'none';
                }else{
                    opt.style.display = 'inline-block';
                    document.getElementById(opt.name + "Label").style.display = 'inline-block';
                }
            }
        });
    }

    function generateInputs(endpoint, options){
        for (var key in options) {
            let entry = options[key];

            var inputType = '';

            switch(entry.type){
                case "bool":
                inputType = 'checkbox';
                break;
                case "int":
                inputType = 'number';
                break;
                case "text":
                case "color": //todo
                inputType = 'text';
                break;
                default:
                console.log("No input type found for " + entry.type);
                inputType = 'text';
                break;
            }

            var inputHtml = "<tr><td><input onkeypress='return handleKey(event)' data-endpoint='wow/" + endpoint + "' class='" + endpoint + "Option opt' id='" + key + "Input' type='" + inputType + "' name='" + key + "'></td><td><label id='" + key + "Label' for='" + key + "Input'>" + key + "</label></td></tr>";

            document.getElementById(endpoint + "Table").insertAdjacentHTML('beforeend', inputHtml);

            if(entry.type == "bool"){
                if(entry.default == "true"){
                    document.getElementById(key + "Input").checked = true;
                }
            }
        }
    }

    function handleKey(event){
        if(event.keyCode == 13){
            sendRequest();
        }
    }

    /* Form submit / render */
    function sendRequest(){
        var arg1 = document.getElementById("arg1").value;
        if(arg1.length == 0){
            console.log("invalid arg");
            return;
        }

        var firstGet = true;
        var queryString = '';

        var opts = document.querySelectorAll('.opt');
        Array.prototype.forEach.call(opts, function(opt) {
            // check if value isnt empty
            if(opt.value){
                // Are we at the start of queryString? Start with ?, otherwise start with &
                if(firstGet){
                    queryString += '?';
                    firstGet = false;
                }else{
                    queryString += '&';
                }
                // Handle checkboxes differently (opt.value's are always "on" apparently)
                if(opt.type == "checkbox"){
                    queryString += opt.name + "=" + opt.checked;
                }else{
                    queryString += opt.name + "=" + opt.value;
                }
            }
        });

        var url = renderServiceURL + endPoint + "/" + arg1 + queryString;

        console.log(url);
        document.getElementById("canvas").src = url;
    }

