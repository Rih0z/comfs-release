puts "please set up client"
senddata = gets
file = File.open("/Users/rih0z/study/comFS/m/TCP4/localhost/8888/opt", "r+") 
puts "please input message"
senddata = gets
file.puts(senddata)
file.flush
senddata = file.read
puts senddata
