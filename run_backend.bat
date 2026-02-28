@echo off
cd Backend
if not exist node_modules (
  echo Installing dependencies...
  npm install
)
echo Starting backend server...
npm run start
