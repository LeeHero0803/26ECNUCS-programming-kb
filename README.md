# 程序设计课堂知识库

使用开源文档框架 [VitePress](https://vitepress.dev/zh/) 1.6.4 构建，输出纯静态网站。无需数据库或后端；字体、配图、搜索索引与脚本随网站托管，不依赖第三方 CDN。

## 本地使用

安装 Node.js 22 LTS 或更高版本，在本目录运行：

```sh
npm ci
npm run docs:dev
```

生成网站：`npm run docs:build`。产物在 `dist/`。预览产物：`npm run docs:preview`。

## 修改与增加文章

- `docs/index.md`：首篇教程，首页直接显示正文。
- `docs/resources.md`：资源下载页。
- `docs/img/`：文章配图，已转为 WebP，44 张约 4.39 MB。
- `docs/public/downloads/`：两份原始 C 源码和未经修改的原始 Markdown。
- `docs/.vitepress/config.mts`：站点名称、导航、目录和中文本地搜索。
- `docs/.vitepress/theme/style.css`：基于默认主题的小幅样式调整。

新增文章可创建 `docs/新文章.md`，配图放入 `docs/img/`，并在配置中的 `sidebar` 增加链接。每次修改后重新构建、发布即可。源码放到 `docs/public/downloads/`，参照资源页添加下载链接。

迁移时保留了教程文字、代码、表格和数学着色；规范了提示框大小写、图片标签及居中容器，增加了源码下载链接。未执行讲义中的安装命令，也未进行技术内容校订。原文中的软件安装包和 PDF 未作为网站资源提供；上传的压缩包未包含 CodeBlocks 软件安装包。

## 免费托管：GitHub Pages

GitHub Free 支持公开仓库的 Pages，参见 [官方说明](https://docs.github.com/en/pages/getting-started-with-github-pages)。仓库内容会公开，请仅放希望公开的课堂材料。

1. 在自己的 GitHub 账号创建公开仓库，将本项目推送到 `main` 分支，不上传 `node_modules`。
2. 在仓库 Settings → Pages → Build and deployment，将 Source 设为 GitHub Actions。
3. 已附 `.github/workflows/pages.yml`，推送后自动构建发布，完成后在 Pages 中取得网址。

工作流会自动读取 Pages 的路径前缀，支持仓库子目录网址与自定义域名。修改 Markdown 后推送即可更新。

## 免费托管：Cloudflare Pages

参考 [官方 VitePress 部署指南](https://developers.cloudflare.com/pages/framework-guides/deploy-a-vitepress-site/)。连接本项目仓库，构建命令填 `npm run docs:build`，输出目录填 `dist`，Node.js 版本设为 22。也可把生成的 `dist` 目录通过 Pages Direct Upload 上传。

## 文件与使用权

框架使用 VitePress 自身的开源许可。课程文章、配图和示例程序仍归原作者所有，本项目未擅自为这些内容授予开源许可。
