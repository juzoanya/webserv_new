import cgi, os

# Set the directory you want to upload to
upload_dir = 'uploads/'

# Make sure the directory exists
if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

form = cgi.FieldStorage()

# Get the file data
fileitem = form['file']

# Check if the file was uploaded
if fileitem.filename:
    # Strip leading path from file name to avoid directory traversal attacks
    fn = os.path.basename(fileitem.filename)
    with open(os.path.join(upload_dir, fn), 'wb') as f:
        # Write the file to the upload directory
        f.write(fileitem.file.read())
    message = 'The file "' + fn + '" was uploaded successfully'
else:
    message = 'No file was uploaded'

# print("status: 404 Not found\r\n", end="")
# print('Content-Type: text/html\r\n', end="")
# print('\r\n', end="")
# print("HELLO WORLDDDDDDDDDD")
# print('<html><head></head><body>')
# print('<h1>' "HEY!" '</h1>')
# print('</body></html>')

print ("Content-type:text/html\r\n")
print ('\r\n')
print ('<html>')
print ('<head>')
print ('<title>Upload</title>')
print ('</head>')
print ('<body>')
print ('<h2>' + message + '</h2>')
print ('</body>')
print ('</html>')
