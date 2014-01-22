BROKER_URL = "mongodb://arbor/celery"
CELERY_RESULT_BACKEND = "mongodb"
CELERY_MONGODB_BACKEND_SETTINGS = {
    "host": "arbor",
    "database": "celery"
}
