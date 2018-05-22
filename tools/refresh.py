#!/usr/bin/env python

import psycopg2 as db
import os, sys

connection_info = dict(database="proxy_db",user="proxy",password="yoidUct0", host="10.6.143.9")
connection = None

directions = ['/var/squid/logs/access.log','/var/log/squid/access.log','/usr/local/squid/var/logs/access.log']
lines = []
for path in directions:
    if os.path.exists(path):
        f = open(path, 'r')
        lines = list(f)
        f.close()
quotas = {}

if not os.path.exists('.lastline'):
    with open('.lastline', 'w') as lastline:
        lastline.write(lines[0])
        lastline.write('0')
    
flast = open('.lastline', 'r')
data = list(flast)
first = data[0]
last = int(data[1].strip())
flast.close()
if first != lines[0]:
    with open('.lastline', 'w') as lastline:
        lastline.write(lines[0])
        lastline.write('0')
print "No Changes"

for line in lines[last:]:
    print line
    date, duration, ip, status, bytesc, oper, url, user, method, cont_type = line.split()
    if user != '-' and not status.__contains__('TCP_DENIED/') and not status.__contains__('NONE/'):
        if not quotas.has_key(user):
            quotas[user] = int(bytesc)
        else:
            quotas[user] += int(bytesc)
    last += 1

with open('.lastline', 'w') as lastline:
    lastline.write(lines[0])
    lastline.write(str(last))
# print quotas

for user in quotas.keys():
    query = "SELECT consumido FROM estudiantes_dean WHERE correo = %s"
    query_args = (user)
    try:
        if connection is None:
            connection = db.connect(**connection_info)
        cursor = connection.cursor()
        cursor.execute(query, query_args)
    except Exception, e:
        connection = db.connect(**connection_info)
        cursor = connection.cursor()
        cursor.execute(query, query_args)
    try:
        row = cursor.fetchone()
        if row is not None:
            print quotas[user] + int(row[0])
            query = "UPDATE estudiantes_dean SET consumido = %s WHERE correo = %s"
            query_args = (quotas[user] + int(row[0]), user)
            cursor.execute(query, query_args)
            connection.commit()
        else:
            print "Something wrong"
            sys.exit(2)
    except:
        print "Something wrong"
        sys.exit(2)
    finally:
        if connection:
            print "Finally"
            connection.close()