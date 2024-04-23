require 'socket'

# Tempo inicial do programa
starttime = Process.clock_gettime(Process::CLOCK_MONOTONIC)

# Criando objeto TCPSocket e conectando-se ao localhost na porta 8989
s = TCPSocket.new 'localhost', 8989

# Envia uma mensagem ao servidor requisitando uma das testfiles requisitadas no argumento
s.write("/tmp/testfiles/#{ARGV[0]}.txt\n")

# Imprime cada linha da mensagem recebida pelo servidor
s.each_line do |line|
    puts line
end

# Fecha a conexão com o socket após a leitura das linhas
s.close

# Tempo final do programa
endtime = Process.clock_gettime(Process::CLOCK_MONOTONIC)
elapsed = endtime - starttime

# Imprime o tempo de duração do programa e qual o arquivo foi pedido
puts "Elapsed: #{elapsed} (#{ARGV[0]})"