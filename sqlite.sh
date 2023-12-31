#!/bin/bash

# Caminho para o banco de dados SQLite
DATABASE="playlist.db"

# Arquivo de trava para verificar se o script já está em execução
LOCK_FILE="/tmp/playlist_import.lock"

# Verifica se o arquivo de trava existe (indicando que o script já está em execução)
if [ -e "$LOCK_FILE" ]; then
    echo "O script já está em execução. Saindo."
    exit 1
fi
# Cria o arquivo de trava
touch "$LOCK_FILE"


# Verifica se o banco de dados já existe, se não, cria a tabela
if [ ! -f "$DATABASE" ]; then
    sqlite3 "$DATABASE" <<EOF
    CREATE TABLE musics (
        id TEXT PRIMARY KEY,
        path_file TEXT,
        duration INTEGER,
        file_name TEXT,
        rank INTEGER,
        percent INTEGER,
        last_played DATETIME,
        bytes INTEGER
    );
EOF
fi

# Diretório a ser pesquisado (substitua pelo caminho desejado)
DIRECTORY="/media/"

# Use o comando find para obter a lista de arquivos
find "$DIRECTORY" -type f | while read -r FILE; do
    # Gera valores fictícios para duration, rank, percent, last_played e bytes
    DURATION=$((RANDOM % 300))
    RANK=$((RANDOM % 100))
    PERCENT=$((RANDOM % 100))
    LAST_PLAYED=$(date -R)
    BYTES=$(wc -c < "$FILE")

    # Gera um ID único com base no caminho do arquivo
    ID=$(echo -n "$FILE" | md5sum | cut -d ' ' -f 1)

    # Executa a inserção no banco de dados
    sqlite3 "$DATABASE" <<EOF
    INSERT INTO musics (id, path_file, duration, file_name, rank, percent, last_played, bytes)
    VALUES ('$ID', '$FILE', $DURATION, '$(basename "$FILE")', $RANK, $PERCENT, '$LAST_PLAYED', $BYTES);
EOF
done

echo "Inserção concluída."

# Remova o arquivo de trava no final do script
trap 'rm -f "$LOCK_FILE"' EXIT


