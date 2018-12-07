mysql -u cod_reader -h www.crystallography.net cod -e 'select sg, sgHall, a, b, c, alpha, beta, gamma from data' --batch > crystallography.tsv
