set_port(443)
set_https(true)

add_cer("localhost", "./localhost.key", "./localhost.cer")
add_module("localhost", "./LiteHttpd.FileServer")
