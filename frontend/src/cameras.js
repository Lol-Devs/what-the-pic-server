import axios from 'axios';

axios.defaults.baseURL = 'http://localhost:8000/api/v1';

export default {
    getAllCameras() {
        return axios.get('/camera').then(
            response => response.data
        );
    },

    getCamera(id) {
        return axios.get('/camera/' + id).then(
            response => response.data
        );
    }
}