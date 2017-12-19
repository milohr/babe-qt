
var wsUri = "ws://localhost:8483";
var websocket = null;

document.addEventListener('DOMContentLoaded', function()
{
    var babeBtn = document.getElementById('babe');
    var addBtn = document.getElementById('add');

    chrome.tabs.query({ currentWindow: true, active: true }, function (tab)
    {
        var d = document;
        var url = tab[0].url;
        var ydoc = tab[0].title;

        console.log(ydoc);


        if (url.includes("youtube.com/watch?v"))
        {
            initWebSocket();

            var babeTitle = document.getElementById('title');
            var babeArtist = document.getElementById('artist');
            var babeAlbum = document.getElementById('album');

            var pageUrl = tab[0].url;
            var pageTitle = tab[0].title;

            if (pageUrl.includes("youtube.com/watch?v"))
            {
                var regex;
                var titleArray = pageTitle.split("-");
                var newTitle = titleArray[1];
                var newArtist = titleArray[0];

                if(pageTitle.indexOf(')')>-1)
                {
                    regex = / *\([^)]*\) */g;
                    newTitle = newTitle.replace(regex, " ").trim();
                    newArtist = newArtist.replace(regex, " ").trim();
                }

                if(pageTitle.indexOf('[')>-1)
                {
                    regex = / *\[[^)]*\] */g;
                    newTitle = newTitle.replace(regex, " ").trim();
                    newArtist = newArtist.replace(regex, " ").trim();
                }

                regex = /"/g
                if(newTitle.indexOf('"')>-1)
                    newTitle = newTitle.replace(regex,"").trim();

                if(newArtist.indexOf('"')>-1)
                    newArtist = newTitle.replace(regex,"").trim();


                babeTitle.value = newTitle;
                babeArtist.value = newArtist;

                chrome.browserAction.setIcon({ path: "icon_done.png" });
            } else
                chrome.browserAction.setIcon({ path: "icon_1.png" });


            var id = url.substring(url.lastIndexOf("watch?v=") + "watch?v=".length, url.length);

            if (id.includes("&"))
                id = id.substring(0, id.indexOf("&"));

            if (id.includes("#"))
                id = id.substring(0, id.indexOf("#"));

            babeBtn.addEventListener('click', function()
            {
                pageUrl = tab[0].url.replace("- YouTube", "");

                var title = document.getElementById('title').value;
                var artist = document.getElementById('artist').value;
                var album = document.getElementById('album').value;
                var playlist = document.getElementById("playlist").value;

                var json = {
                    title: title.trim(),
                    artist: artist.trim(),
                    album: album.trim(),
                    playlist: playlist.trim(),
                    babe: 1,
                    id: id.trim(),
                    page: pageTitle.trim()
                }

                sendData(json);
            });

        }else
            document.getElementById("warning").innerHTML += "This isn't a YouTube url";
    });

});

function sendData(json)
{
    console.log(json);

    if (websocket != null)
        websocket.send( JSON.stringify(json) );

}

function initWebSocket()
{
    try
    {
        if (typeof MozWebSocket == 'function')
            WebSocket = MozWebSocket;
        if ( websocket && websocket.readyState == 1 )
            websocket.close();
        websocket = new WebSocket( wsUri );
        websocket.onopen = function (evt)
        {
            console.log("CONNECTED");
        };
        websocket.onclose = function (evt)
        {
            console.log("DISCONNECTED");
        };
        websocket.onmessage = function (evt)
        {
            console.log( "Message received :", evt.data );
            console.log(evt.data);
            setPlaylists(evt.data);

        };
        websocket.onerror = function (evt)
        {
            document.getElementById("warning").innerHTML = 'SERVER ERROR: ' + evt.data;
        };
    }catch (exception)
    {
        document.getElementById("warning").innerHTML = 'SERVER ERROR: ' + exception;
    }
}

function stopWebSocket()
{
    if (websocket)
        websocket.close();
}

function checkSocket()
{
    if (websocket != null)
    {
        var stateStr;
        switch (websocket.readyState)
        {
        case 0:
            stateStr = "CONNECTING";
            break;

        case 1:
            stateStr = "OPEN";
            break;

        case 2:
            stateStr = "CLOSING";
            break;

        case 3:
            stateStr = "CLOSED";
            break;

        default:
            stateStr = "UNKNOW";
            break;
        }

        console.log("WebSocket state = " + websocket.readyState + " ( " + stateStr + " )");
    }else
        console.log("WebSocket is null");

}

function setPlaylists(playlists)
{
    select = document.getElementById('playlist');
    list = playlists.split(",");
    for(i = 0; i < list.length; i++)
    {
        var opt = document.createElement('option');
        opt.value = list[i];
        opt.innerHTML = list[i];
        select.appendChild(opt);
    }

}

function download(strData, strFileName, strMimeType)
{
    var D = document,
            A = arguments,
            a = D.createElement("a"),
            d = A[0],
            n = A[1],
            t = A[2] || "text/plain";

    //build download link:
    a.href = "data:" + strMimeType + "charset=utf-8," + escape(strData);


    if (window.MSBlobBuilder) { // IE10
        var bb = new MSBlobBuilder();
        bb.append(strData);
        return navigator.msSaveBlob(bb, strFileName);
    } /* end if(window.MSBlobBuilder) */



    if ('download' in a) { //FF20, CH19
        a.setAttribute("download", n);
        /* a.innerHTML = "downloading...";*/
        D.body.appendChild(a);
        setTimeout(function () {
            var e = D.createEvent("MouseEvents");
            e.initMouseEvent("click", true, false, window, 0, 0, 0, 0, 0, false, false, false, false, 0, null);
            a.dispatchEvent(e);
            D.body.removeChild(a);
        }, 66);
        return true;
    }; /* end if('download' in a) */



    //do iframe dataURL download: (older W3)
    var f = D.createElement("iframe");
    D.body.appendChild(f);
    f.src = "data:" + (A[2] ? A[2] : "application/octet-stream") + (window.btoa ? ";base64" : "") + "," + (window.btoa ? window.btoa : escape)(strData);
    setTimeout(function () {
        D.body.removeChild(f);
    }, 333);
    return true;
}
