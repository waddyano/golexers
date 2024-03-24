package main

import (
	"flag"
	"fmt"
	"io"
	"net/http"
	"os"
)

func indexHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Printf("request: %s\n", r.URL.Path)
	html := `
	<html>
	<head>
	<style>
	#droparea {
		border: 2px dashed #ccc;
		border-radius: 20px;
		width: 480px;
		font-family: sans-serif;
		padding: 20px;
	}
	#drop-area.highlight {
		border-color: purple;
	}
	.is-dragover {
		background-color: lightgrey;
	}
	</style>
	%s
	<script>
	function handleFiles(files) {
		uploadFile(files[0])
	}
	function uploadFile(file) {
		let url = '/upload'
		let formData = new FormData()
	  
		formData.append('file', file)
	  
		fetch(url, {
		  method: 'POST',
		  body: formData
		})
		.then((response) => { return response.text() })
		.then((data) => { formatted.innerHTML = data })
		.catch((e) => { console.log(e) })
	}
	function dragOverHandler(ev) {
		ev.preventDefault();
		droparea.classList.add('is-dragover')
  	}
	function dragLeaveHandler(ev) {
		droparea.classList.remove('is-dragover')
  	}
	function dropHandler(ev) {
		droparea.classList.remove('is-dragover')
		ev.preventDefault();
	  
		[...ev.dataTransfer.items].forEach((item, i) => {
		// If dropped items aren't files, reject them
		if (item.kind === "file") {
			const file = item.getAsFile();
			uploadFile(file);
		}
		});
	}
    </script>
	</head>
	<body>
	<div id="droparea" ondrop="dropHandler(event);" ondragover="dragOverHandler(event);" ondragleave="dragLeaveHandler(event);">
	<form class="my-form">
	  <p>Upload a file with the file dialog or by dragging and dropping a file onto the dashed region or choosing a file</p>
	  <input type="file" id="fileElem" onchange="handleFiles(this.files)">
	</form>
  	</div>
	<div id="formatted">
	</div>
  	</body></html>`
	// Handles top-level page.
	w.Header().Add("Content-Type", "text/html")
	fmt.Fprintf(w, html, style)
}

func uploadHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Printf("upload request: %s\n", r.URL.Path)
	if err := r.ParseMultipartForm(10000000); err != nil {
		fmt.Printf("ParseForm() err: %v", err)
		return
	}
	file, header, err := r.FormFile("file")
	if err != nil {
		return
	}
	defer file.Close()
	d, _ := io.ReadAll(file)
	options := FormatOptions{LineNumbers: true}
	w.Header().Add("Content-Type", "text/html")
	formatCode(w, options, header.Filename, d)
}

func runServer(port int) {
	http.HandleFunc("GET /{$}", indexHandler)
	http.HandleFunc("POST /upload", uploadHandler)
	http.ListenAndServe(fmt.Sprintf(":%d", port), nil)
}

func main() {
	port := flag.Int("port", 9000, "port for web server run by -s")
	server := flag.Bool("s", false, "run web server")
	flag.Parse()

	if *server {
		runServer(*port)
		return
	}
	if flag.NArg() < 1 {
		fmt.Fprintf(os.Stderr, "must specify path to format\n")
		os.Exit(1)
	}

	filename := flag.Arg(0)
	options := FormatOptions{LineNumbers: true}
	err := Format(filename, options, os.Stdout)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed: %s\n", err.Error())
	}
}
