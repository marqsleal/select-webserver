#!/bin/bash

# Array contendo os parágrafos do texto "Lorem Ipsum"
lorem_ipsum=(
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit."
    "Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas."
    "Quisque pretium, lacus nec sollicitudin condimentum, eros mi dapibus sem, sed commodo libero libero ut urna."
    "Vivamus at sapien a lorem tempor dictum."
    "Nullam tincidunt tristique neque, nec fermentum arcu auctor ac."
)

# Criar o diretório "testfiles" em /tmp, se ainda não existir
mkdir -p /tmp/testfiles

# Loop para criar 5 arquivos de texto com parágrafos diferentes do texto "Lorem Ipsum"
for i in {1..5}; do
    # Selecionar o parágrafo correspondente ao índice do loop
    paragraph="${lorem_ipsum[$i - 1]}"
    echo "$paragraph" > "/tmp/testfiles/$i.txt"
done

echo "Arquivos de texto criados com sucesso em /tmp/testfiles."
