#!/usr/bin/python

""" A script for sending emails with results of nightly builds

This code is licensed under the New BSD license.
See LICENSE.txt for details

This script has 3 expectations.

1. A commandline argument which is a brief message which will go into the
   subject of the email.
2. A file next to wherever this script is installed called UserPassword.json
   which is a json file in it with an entry for username and password.
   {
    "username":"______@gmail.com",
    "password":"The kind of password an idiot would have on his luggage"
    }
3. That the email you're sending from is gmail. 
 """

import smtplib
import json
import argparse
from email.MIMEMultipart import MIMEMultipart
from email.MIMEBase import MIMEBase
from email.MIMEText import MIMEText
from email import Encoders
import os

def readUserAndPassword(filename):
    """ Read in the username and password from a given file
    """
    with open(filename,"r") as f:
        data = json.load(f)
        return data

def sendMail(to, subject, text, sender, sender_pw):
    """ Send a mail from sender@gmail to to with subject and text.
    """
    msg = MIMEMultipart()

    msg['From'] = sender 
    msg['To'] = to
    msg['Subject'] = subject 

    msg.attach(MIMEText(text))

    mailServer = smtplib.SMTP('smtp.gmail.com', 587)
    mailServer.ehlo()
    mailServer.starttls()
    mailServer.ehlo()
    mailServer.login(sender, sender_pw)
    mailServer.sendmail(sender, to, msg.as_string())
    mailServer.close()

parser = argparse.ArgumentParser(description="Mail Nightly test results")
parser.add_argument("msg")
args = parser.parse_args()
loginInfo = readUserAndPassword("UserPassword.json")
sendMail("loarabia@hotmail.com", args.msg, "", 
    loginInfo["username"], loginInfo["password"])
