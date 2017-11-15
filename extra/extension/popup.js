document.addEventListener('DOMContentLoaded', function () {
    var babeBtn = document.getElementById('babe');
    var addBtn = document.getElementById('add');

    chrome.tabs.getSelected(null, function (tab) {

        var babeTitle = document.getElementById('title');
        var babeArtist = document.getElementById('artist');
        var babeAlbum = document.getElementById('album');

        var pageUrl = tab.url;
        var pageTitle = tab.title;

        if (pageUrl.includes("youtube.com/watch?v")) {
            var regex = / *\([^)]*\) */g;
            var title = pageTitle.split("-");
            babeTitle.value = title[1].replace(regex, " ").trim();
            babeArtist.value = title[0].replace(regex, " ").trim();
            chrome.browserAction.setIcon({
                path: "icon_done.png"
            });
        } else {

            chrome.browserAction.setIcon({
                path: "icon_1.png"
            });
        }

    });

    babeBtn.addEventListener('click', function () {

        chrome.tabs.getSelected(null, function (tab) {
            d = document;
            var url = tab.url;
            var ydoc = tab.title;
            console.log(ydoc);
            if (url.includes("youtube.com/watch?v")) {
                var id = url.substring(url.lastIndexOf("watch?v=") + 8, url.length);

                if (id.includes("&")) {
                    console.log("the string inclues ambersan");
                    id = id.substring(0, id.indexOf("&"));
                } else if (id.includes("#")) {
                    console.log("the string inclues #");
                    id = id.substring(0, id.indexOf("#"));

                }


                var pageUrl = tab.url.replace("- YouTube", "");
                var pageTitle = tab.title;
                var title = document.getElementById('title').value;
                var artist = document.getElementById('artist').value;
                var album = document.getElementById('album').value;
                var info = "[title] = " + title + "\n[artist] = " + artist + "\n[album] = " + album + "\n[babe] = 1" + "\n[id] = " + id + "\n[page] = " + pageTitle;

                download(info, id + '.babe', 'text/plain');


            } else {
                console.log("url does not contains yuotube");

                document.getElementById("warning").innerHTML += "This isn't a YouTube url";
            }

        });
    }, false);

    addBtn.addEventListener('click', function () {

        chrome.tabs.getSelected(null, function (tab) {
            d = document;
            var url = tab.url;
            var ydoc = tab.title;
            console.log(ydoc);
            if (url.includes("youtube.com/watch?v")) {
                var id = url.substring(url.lastIndexOf("watch?v=") + 8, url.length);

                if (id.includes("&")) {
                    console.log("the string inclues ambersan");
                    id = newStr.substring(0, id.indexOf("&"));
                } else if (id.includes("#")) {
                    console.log("the string inclues #");
                    id = newStr.substring(0, id.indexOf("#"));

                }

                var pageUrl = tab.url;
                var pageTitle = tab.title;
                var title = document.getElementById('title').value;
                var artist = document.getElementById('artist').value;
                var album = document.getElementById('album').value;
                var info = "[title] = " + title + "\n[artist] = " + artist + "\n[album] = " + album + "\n[babe] = 0" + "\n[id] = " + id + "\n[page] = " + pageTitle;

                download(info, id + '.babe', 'text/plain');


            } else {
                console.log("url does not contains yuotube");

                document.getElementById("warning").innerHTML += "This isn't a YouTube url";
            }

        });
    }, false);


}, false);




function download(strData, strFileName, strMimeType) {
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
