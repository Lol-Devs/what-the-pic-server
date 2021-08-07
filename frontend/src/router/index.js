import { createWebHistory, createRouter } from "vue-router";
import Home from "../views/Home.vue";
import Cam from "../views/Cam.vue";

const routes = [
  {
    path: "/",
    name: "Home",
    component: Home,
  },
  {
    path: "/cam/:id",
    name: "Cam",
    component: Cam,
  },
  
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

export default router;