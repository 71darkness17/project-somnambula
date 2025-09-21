from fastapi import FastAPI, Depends, HTTPException, status, Query
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from jose import JWTError, jwt
from passlib.context import CryptContext
from pydantic import BaseModel
from datetime import datetime, timedelta
from typing import Optional
from fastapi.middleware.cors import CORSMiddleware
import secrets
import base64



SECRET_KEY = secrets.token_urlsafe(32)
ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 30

server = FastAPI()

server.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # или ["http://localhost:3000"]
    allow_credentials=True,
    allow_methods=["*"],  # POST, GET, OPTIONS и т.д.
    allow_headers=["*"],
)

class UserAuth(BaseModel):
    email: str
    password: str

class Log(BaseModel):
    url: str
    status_code: int
    timestamp: float
    response_time: float
    redirs: int
    ssl: bool

class User(BaseModel):
    email: str
    username: Optional[str]
    telegram: Optional[str]
    phone_number: Optional[str]

class DbUser(User):
    hash_password: str

class UserCreate(BaseModel):
    email: str
    username: str
    password: str

class Token(BaseModel):
    access_token: str
    token_type: str

class TokenData(BaseModel):
    username: Optional[str] = None

pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")


oneTimeDB = {
    "dalepa@edu.hse.ru": {
        "email": "dalepa@edu.hse.ru",
        "username": "Fatum",
        "hash_password": pwd_context.hash("dalepa"),
        "telegram": "@fatummm",
        "phone_number": "+77777777777"
    }
}

oneTimeBindDb = {
    "abcd": {
        "id": "abcd",
        "email": "dalepa@edu.hse.ru",
        "url": "https://google.com",
        "freq": 60,
        "notify": False
    }
}

oneTimeLogDb = [
    {
        "bind_id": "abcd",
        "status_code": 200,
        "timestamp": 1.0,
        "response_time": 1.0,
        "redirs": 1,
        "ssl": True
    }
]

def verify_pwd(pwd, hashed_password):
    return pwd_context.verify(pwd,hashed_password)

def pwd_hash(pwd: str):
    return pwd_context.hash(pwd)

def get_user(username: str):
    if username in oneTimeDB:
        user = oneTimeDB[username]
        return DbUser(**user)

def create_user(user: UserCreate):
    if user.email in oneTimeDB:
        return 0
    oneTimeDB[user.email] = {"email": user.email,
                             "username": user.username,
                             "hash_password": pwd_context.hash(user.password),
                             "telegram": "",
                             "phone_number": ""}
    
def auth_user(username: str, password: str):
    user = get_user(username)
    if not user: 
        return 1
    if not verify_pwd(password,user.hash_password):
        return 2
    return user

def create_access_token(data: dict, expires_delta: Optional[timedelta] = None):
    to_encode = data.copy()
    if expires_delta:
        expire = datetime.utcnow() + expires_delta
    else:
        expire = datetime.utcnow() + timedelta(minutes=15)
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode,SECRET_KEY, algorithm=ALGORITHM)
    return encoded_jwt

async def get_curren_user(token: str = Depends(oauth2_scheme)):
    cr_except = HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Could not validate credentials",
        headers={"WWW-Authenticate": "Bearer"},
    )
    try:
        payload = jwt.decode(token,SECRET_KEY,algorithms=[ALGORITHM])
        username: str = payload.get("sub")
        if username is None:
            raise cr_except
        token_data = TokenData(username=username)
    except JWTError:
        raise cr_except
    user = get_user(token_data.username)
    if user is None:
        raise cr_except
    return user

@server.post("/token", response_model=Token)
async def auth_for_token(form: UserAuth):
    user = auth_user(form.email,form.password)
    if user == 1:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Incorrect username or password",
            headers={"WWW-Authenticate": "Bearer"},
        )
    if user == 2:
        raise HTTPException(
            status_code=status.HTTP_402_PAYMENT_REQUIRED,
            detail="Incorrect username or password",
            headers={"WWW-Authenticate": "Bearer"},
        )
    token_expires = timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    token = create_access_token(data={"sub": user.username}, expires_delta=token_expires)
    return {"access_token": token, "token_type": "bearer"}

@server.get("users/self",response_model=User)
async def get_user_self(curren_user: User = Depends(get_curren_user)):
    return curren_user

# @server.get("/log",response_model=list[Log])
# async def get_user_log(user: User = Depends(get_curren_user))

@server.get("log/last",response_model=list[Log])
async def get_user_last_log(user: User = Depends(get_curren_user),
                            url: str = Query(10,alias="url",description="Url to find logs"),
                            num: Optional[int] = Query(10,alias="number",description="Set the amount of last n logs")):
    log_list = list()
    log = Log()
    return log

@server.options("/token/register")
async def allow_opt():
    raise HTTPException(
        status_code=status.HTTP_200_OK
    )

@server.post("/token/register", response_model=Token)
async def register_user(create_data: UserCreate):
    if create_user(create_data) == 0:
        raise HTTPException(
            status_code=status.HTTP_409_CONFLICT,
            detail="This email already exists"
        )
    user = auth_user(create_data.email,create_data.password)
    if not user:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Fucked up"
        )
    token_expires = timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    token = create_access_token(data={"sub": user.username}, expires_delta=token_expires)
    return {"access_token": token, "token_type": "bearer"}
    
